/**
 * @file web_main.cpp
 * @brief Web 版本入口 - HTTP 服务器
 *
 * 使用方法：
 *   ./HarryPotterWeb [port]
 *   默认端口: 8080
 *
 * 兼容参数（可选）：
 *   data_dir 仅用于旧接口 /api/load?file=... 的相对路径解析
 *
 * 访问：http://localhost:8080
 */

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cctype>

#include "web/HttpServer.h"
#include "core/TextProcessor.h"
#include "core/InvertedIndex.h"
#include "core/SearchEngine.h"
#include "core/Trie.h"
#include "core/Similarity.h"
#include "core/StringMatch.h"

// 全局变量（简化实现）
InvertedIndex* g_index = nullptr;
SearchEngine* g_searchEngine = nullptr;
Trie* g_trie = nullptr;
Similarity* g_similarity = nullptr;
TextProcessor* g_processor = nullptr;
std::vector<std::vector<std::string>> g_bookContents;
std::vector<std::string> g_bookNames;
std::string g_dataDir = "data";
int g_nextDocId = 1;

// JSON 转义
std::string jsonEscape(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}

std::string deriveBookName(const std::string& filename) {
    std::string name = filename;
    size_t lastSlash = name.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        name = name.substr(lastSlash + 1);
    }

    size_t dotPos = name.find_last_of('.');
    if (dotPos != std::string::npos) {
        name = name.substr(0, dotPos);
    }

    if (name.empty()) {
        name = "uploaded_text";
    }
    return name;
}

std::vector<std::string> splitTextLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream in(text);
    std::string line;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }

    if (lines.empty() && !text.empty()) {
        lines.push_back(text);
    }

    return lines;
}

// 重建 Trie
void rebuildTrie() {
    delete g_trie;
    g_trie = new Trie();
    auto topTerms = g_index->getTopTerms(5000);
    for (const auto& term : topTerms) {
        g_trie->insert(term.first, term.second);
    }
}

void rebuildSearchComponents() {
    delete g_searchEngine;
    delete g_similarity;
    g_searchEngine = new SearchEngine(*g_index);
    g_similarity = new Similarity(*g_index);
}

int addBookToIndex(const std::string& bookName, const std::vector<std::string>& lines) {
    int docId = g_nextDocId++;
    g_index->addDocument(docId, bookName, lines);
    g_bookContents.push_back(lines);
    g_bookNames.push_back(bookName);

    rebuildTrie();
    rebuildSearchComponents();
    return docId;
}

// API: 加载文件
void handleLoad(const HttpRequest& req, HttpResponse& res) {
    std::string filename = req.params.count("file") ? req.params.at("file") : "";
    std::string bookName = req.params.count("name") ? req.params.at("name") : "";

    if (filename.empty()) {
        res.setJson("{\"error\": \"Missing parameter 'file'\"}");
        return;
    }

    // 构建完整路径
    std::string filePath = filename;
    if (filename[0] != '/' && filename.find('/') == std::string::npos) {
        filePath = g_dataDir + "/" + filename;
    }

    // 如果没有提供书名，使用文件名
    if (bookName.empty()) {
        bookName = deriveBookName(filename);
    }

    try {
        auto lines = g_processor->readLines(filePath);
        if (lines.empty()) {
            res.setJson("{\"error\": \"File is empty or cannot be read\"}");
            return;
        }

        int docId = addBookToIndex(bookName, lines);

        std::ostringstream json;
        json << "{\"success\": true, \"docId\": " << docId
             << ", \"name\": \"" << jsonEscape(bookName) << "\""
             << ", \"lines\": " << lines.size()
             << ", \"vocabulary\": " << g_index->getVocabularySize()
             << ", \"totalTerms\": " << g_index->getTotalTermCount()
             << "}";
        res.setJson(json.str());

        std::cout << "Loaded: [" << docId << "] " << bookName
                  << " - " << lines.size() << " lines" << std::endl;

    } catch (const std::exception& e) {
        std::ostringstream json;
        json << "{\"error\": \"Failed to load file: " << jsonEscape(e.what()) << "\"}";
        res.setJson(json.str());
    }
}

// API: 上传 txt 内容
void handleUpload(const HttpRequest& req, HttpResponse& res) {
    std::string filename = req.params.count("name") ? req.params.at("name") : "";
    std::string bookName = req.params.count("title") ? req.params.at("title") : "";

    if (filename.empty()) {
        res.setJson("{\"error\": \"Missing query parameter 'name'\"}");
        return;
    }

    if (bookName.empty()) {
        bookName = deriveBookName(filename);
    }

    auto lines = splitTextLines(req.body);
    if (lines.empty()) {
        res.setJson("{\"error\": \"Uploaded file is empty\"}");
        return;
    }

    int docId = addBookToIndex(bookName, lines);

    std::ostringstream json;
    json << "{\"success\": true, \"docId\": " << docId
         << ", \"name\": \"" << jsonEscape(bookName) << "\""
         << ", \"lines\": " << lines.size()
         << ", \"vocabulary\": " << g_index->getVocabularySize()
         << ", \"totalTerms\": " << g_index->getTotalTermCount()
         << "}";
    res.setJson(json.str());

    std::cout << "Uploaded: [" << docId << "] " << bookName
              << " - " << lines.size() << " lines" << std::endl;
}

// API: 列出可用文件
void handleFiles(const HttpRequest&, HttpResponse& res) {
    // 简单实现：返回数据目录信息
    std::ostringstream json;
    json << "{\"dataDir\": \"" << jsonEscape(g_dataDir) << "\""
         << ", \"hint\": \"Use /upload.html or POST /api/upload?name=<filename>.txt to upload files\""
         << ", \"loadedBooks\": [";

    for (size_t i = 1; i <= g_index->getDocumentCount(); ++i) {
        auto info = g_index->getDocumentInfo(i);
        if (i > 1) json << ", ";
        json << "{\"id\": " << i
             << ", \"name\": \"" << jsonEscape(info.name) << "\""
             << ", \"lines\": " << info.totalLines
             << ", \"words\": " << info.totalWords << "}";
    }

    json << "]}";
    res.setJson(json.str());
}

// API: 搜索
void handleSearch(const HttpRequest& req, HttpResponse& res) {
    std::string query = req.params.count("q") ? req.params.at("q") : "";
    std::string mode = req.params.count("mode") ? req.params.at("mode") : "single";

    if (query.empty()) {
        res.setJson("{\"error\": \"Missing query parameter 'q'\"}");
        return;
    }

    std::vector<SearchResult> results;

    if (mode == "and") {
        std::istringstream iss(query);
        std::string w1, w2;
        iss >> w1 >> w2;
        if (!w1.empty() && !w2.empty()) {
            auto postings = g_searchEngine->searchAND(w1, w2);
            for (const auto& p : postings) {
                SearchResult sr;
                sr.docId = p.docId;
                sr.chapter = p.chapter;
                sr.line = p.line;
                results.push_back(sr);
            }
        }
    } else if (mode == "or") {
        std::istringstream iss(query);
        std::string w1, w2;
        iss >> w1 >> w2;
        if (!w1.empty() && !w2.empty()) {
            auto postings = g_searchEngine->searchOR(w1, w2);
            for (const auto& p : postings) {
                SearchResult sr;
                sr.docId = p.docId;
                sr.chapter = p.chapter;
                sr.line = p.line;
                results.push_back(sr);
            }
        }
    } else if (mode == "phrase") {
        auto postings = g_searchEngine->searchPhrase(query);
        for (const auto& p : postings) {
            SearchResult sr;
            sr.docId = p.docId;
            sr.chapter = p.chapter;
            sr.line = p.line;
            results.push_back(sr);
        }
    } else {
        // 单词搜索
        auto postings = g_index->search(query);
        for (const auto& p : postings) {
            SearchResult sr;
            sr.docId = p.docId;
            sr.chapter = p.chapter;
            sr.line = p.line;
            results.push_back(sr);
        }
    }

    // 构建 JSON 响应
    std::ostringstream json;
    json << "{\"query\": \"" << jsonEscape(query) << "\", "
         << "\"mode\": \"" << mode << "\", "
         << "\"count\": " << results.size() << ", "
         << "\"results\": [";

    int limit = std::min(100, static_cast<int>(results.size()));
    for (int i = 0; i < limit; ++i) {
        const auto& r = results[i];
        if (i > 0) json << ", ";

        std::string context;
        int bookIdx = r.docId - 1;
        if (bookIdx >= 0 && bookIdx < static_cast<int>(g_bookContents.size())) {
            int lineIdx = r.line - 1;
            if (lineIdx >= 0 && lineIdx < static_cast<int>(g_bookContents[bookIdx].size())) {
                context = g_bookContents[bookIdx][lineIdx];
            }
        }

        std::string bookName = (bookIdx >= 0 && bookIdx < static_cast<int>(g_bookNames.size()))
            ? g_bookNames[bookIdx] : "Unknown";

        json << "{\"book\": " << r.docId
             << ", \"bookName\": \"" << jsonEscape(bookName) << "\""
             << ", \"chapter\": " << r.chapter
             << ", \"line\": " << r.line
             << ", \"context\": \"" << jsonEscape(context) << "\"}";
    }

    json << "]}";
    res.setJson(json.str());
}

// API: 统计信息
void handleStats(const HttpRequest&, HttpResponse& res) {
    std::ostringstream json;
    json << "{\"documents\": " << g_index->getDocumentCount()
         << ", \"vocabulary\": " << g_index->getVocabularySize()
         << ", \"totalTerms\": " << g_index->getTotalTermCount()
         << ", \"books\": [";

    for (size_t i = 1; i <= g_index->getDocumentCount(); ++i) {
        auto info = g_index->getDocumentInfo(i);
        if (i > 1) json << ", ";
        json << "{\"id\": " << i
             << ", \"name\": \"" << jsonEscape(info.name) << "\""
             << ", \"lines\": " << info.totalLines
             << ", \"words\": " << info.totalWords << "}";
    }

    json << "]}";
    res.setJson(json.str());
}

// API: 词频排行
void handleTopWords(const HttpRequest& req, HttpResponse& res) {
    int n = 20;
    if (req.params.count("n")) {
        n = std::stoi(req.params.at("n"));
    }

    auto topWords = g_index->getTopTerms(n);

    std::ostringstream json;
    json << "{\"words\": [";

    for (size_t i = 0; i < topWords.size(); ++i) {
        if (i > 0) json << ", ";
        json << "{\"word\": \"" << jsonEscape(topWords[i].first) << "\""
             << ", \"count\": " << topWords[i].second << "}";
    }

    json << "]}";
    res.setJson(json.str());
}

// API: 自动补全
void handleAutocomplete(const HttpRequest& req, HttpResponse& res) {
    std::string prefix = req.params.count("prefix") ? req.params.at("prefix") : "";

    if (prefix.empty()) {
        res.setJson("{\"suggestions\": []}");
        return;
    }

    auto suggestions = g_trie->autocomplete(prefix, 10);

    std::ostringstream json;
    json << "{\"suggestions\": [";

    for (size_t i = 0; i < suggestions.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << jsonEscape(suggestions[i]) << "\"";
    }

    json << "]}";
    res.setJson(json.str());
}

// API: 拼写建议
void handleSpelling(const HttpRequest& req, HttpResponse& res) {
    std::string word = req.params.count("word") ? req.params.at("word") : "";

    if (word.empty()) {
        res.setJson("{\"suggestions\": []}");
        return;
    }

    auto suggestions = g_similarity->spellingSuggestions(word, 2, 5);

    std::ostringstream json;
    json << "{\"word\": \"" << jsonEscape(word) << "\", \"suggestions\": [";

    for (size_t i = 0; i < suggestions.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << jsonEscape(suggestions[i]) << "\"";
    }

    json << "]}";
    res.setJson(json.str());
}

// API: 获取上下文（前后段落）
void handleContext(const HttpRequest& req, HttpResponse& res) {
    int docId = req.params.count("doc") ? std::stoi(req.params.at("doc")) : 0;
    int lineNum = req.params.count("line") ? std::stoi(req.params.at("line")) : 0;

    if (docId <= 0 || lineNum <= 0) {
        res.setJson("{\"error\": \"Missing or invalid 'doc' or 'line' parameter\"}");
        return;
    }

    int bookIdx = docId - 1;
    if (bookIdx < 0 || bookIdx >= static_cast<int>(g_bookContents.size())) {
        res.setJson("{\"error\": \"Invalid document ID\"}");
        return;
    }

    const auto& lines = g_bookContents[bookIdx];
    std::string bookName = g_bookNames[bookIdx];
    int lineIdx = lineNum - 1;

    if (lineIdx < 0 || lineIdx >= static_cast<int>(lines.size())) {
        res.setJson("{\"error\": \"Invalid line number\"}");
        return;
    }

    // 找到段落边界（空行分隔）
    // 向前找上一段落
    int prevStart = lineIdx - 1;
    while (prevStart > 0 && !lines[prevStart].empty()) {
        prevStart--;
    }
    int prevEnd = prevStart;
    while (prevEnd > 0 && lines[prevEnd].empty()) {
        prevEnd--;
    }
    int prevParaStart = prevEnd;
    while (prevParaStart > 0 && !lines[prevParaStart - 1].empty()) {
        prevParaStart--;
    }

    // 当前段落
    int currStart = lineIdx;
    while (currStart > 0 && !lines[currStart - 1].empty()) {
        currStart--;
    }
    int currEnd = lineIdx;
    while (currEnd < static_cast<int>(lines.size()) - 1 && !lines[currEnd + 1].empty()) {
        currEnd++;
    }

    // 下一段落
    int nextStart = currEnd + 1;
    while (nextStart < static_cast<int>(lines.size()) && lines[nextStart].empty()) {
        nextStart++;
    }
    int nextEnd = nextStart;
    while (nextEnd < static_cast<int>(lines.size()) - 1 && !lines[nextEnd + 1].empty()) {
        nextEnd++;
    }

    // 构建段落文本
    std::string prevPara, currPara, nextPara;

    if (prevParaStart <= prevEnd && prevEnd >= 0) {
        for (int i = prevParaStart; i <= prevEnd; ++i) {
            if (!prevPara.empty()) prevPara += "\n";
            prevPara += lines[i];
        }
    }

    for (int i = currStart; i <= currEnd && i < static_cast<int>(lines.size()); ++i) {
        if (!currPara.empty()) currPara += "\n";
        currPara += lines[i];
    }

    if (nextStart < static_cast<int>(lines.size())) {
        for (int i = nextStart; i <= nextEnd && i < static_cast<int>(lines.size()); ++i) {
            if (!nextPara.empty()) nextPara += "\n";
            nextPara += lines[i];
        }
    }

    std::ostringstream json;
    json << "{\"docId\": " << docId
         << ", \"bookName\": \"" << jsonEscape(bookName) << "\""
         << ", \"line\": " << lineNum
         << ", \"previousParagraph\": \"" << jsonEscape(prevPara) << "\""
         << ", \"currentParagraph\": \"" << jsonEscape(currPara) << "\""
         << ", \"nextParagraph\": \"" << jsonEscape(nextPara) << "\""
         << "}";
    res.setJson(json.str());
}

// API: 算法对比
void handleCompare(const HttpRequest& req, HttpResponse& res) {
    std::string pattern = req.params.count("pattern") ? req.params.at("pattern") : "";

    if (pattern.empty()) {
        res.setJson("{\"error\": \"Missing 'pattern' parameter\"}");
        return;
    }

    if (g_bookContents.empty()) {
        res.setJson("{\"error\": \"No books loaded\"}");
        return;
    }

    // 合并所有书籍内容
    std::string fullText;
    for (const auto& book : g_bookContents) {
        for (const auto& line : book) {
            fullText += line + "\n";
        }
    }

    StringMatch matcher;
    auto results = matcher.compareAll(fullText, pattern);

    std::ostringstream json;
    json << "{\"pattern\": \"" << jsonEscape(pattern) << "\""
         << ", \"textSize\": " << fullText.length()
         << ", \"algorithms\": [";

    std::vector<std::string> complexities = {"O(nm)", "O(n+m)", "O(n/m)~O(nm)", "O(n+m)"};

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        if (i > 0) json << ", ";
        json << "{\"name\": \"" << r.algorithm << "\""
             << ", \"matches\": " << r.positions.size()
             << ", \"comparisons\": " << r.comparisons
             << ", \"timeMs\": " << std::fixed << std::setprecision(3) << r.timeMs
             << ", \"complexity\": \"" << complexities[i] << "\"}";
    }

    json << "]}";
    res.setJson(json.str());
}

int main(int argc, char* argv[]) {
    int port = 8080;

    if (argc > 1) {
        port = std::stoi(argv[1]);
    }
    if (argc > 2) {
        g_dataDir = argv[2];
    }

    std::cout << R"(
  _   _                          ____       _   _
 | | | | __ _ _ __ _ __ _   _   |  _ \ ___ | |_| |_ ___ _ __
 | |_| |/ _` | '__| '__| | | |  | |_) / _ \| __| __/ _ \ '__|
 |  _  | (_| | |  | |  | |_| |  |  __/ (_) | |_| ||  __/ |
 |_| |_|\__,_|_|  |_|   \__, |  |_|   \___/ \__|\__\___|_|
                        |___/
        Text Retrieval System - Web Interface
    )" << std::endl;

    // 初始化组件
    g_index = new InvertedIndex();
    g_trie = new Trie();
    g_processor = new TextProcessor();
    g_searchEngine = new SearchEngine(*g_index);
    g_similarity = new Similarity(*g_index);

    std::cout << "Legacy data directory for /api/load: " << g_dataDir << std::endl;
    std::cout << "No books loaded. Open /upload.html to upload txt files.\n" << std::endl;

    // 创建 HTTP 服务器
    HttpServer server;

    // 设置静态文件目录
    server.setStaticDir("static");

    // 注册 API 路由
    server.post("/api/upload", handleUpload);
    server.get("/api/load", handleLoad);
    server.get("/api/files", handleFiles);
    server.get("/api/search", handleSearch);
    server.get("/api/context", handleContext);
    server.get("/api/compare", handleCompare);
    server.get("/api/stats", handleStats);
    server.get("/api/top", handleTopWords);
    server.get("/api/autocomplete", handleAutocomplete);
    server.get("/api/spelling", handleSpelling);

    // 启动服务器
    if (!server.start(port)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();

    server.stop();

    // 清理
    delete g_similarity;
    delete g_searchEngine;
    delete g_trie;
    delete g_index;
    delete g_processor;

    return 0;
}
