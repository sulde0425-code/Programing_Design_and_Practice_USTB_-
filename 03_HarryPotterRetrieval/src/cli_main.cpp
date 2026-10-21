/**
 * @file cli_main.cpp
 * @brief 命令行版本 - 用于测试核心算法（无需 Qt 环境）
 *
 * 使用方法：
 *   ./HarryPotterCLI [data_directory]
 *
 * 示例：
 *   ./HarryPotterCLI ./data
 *   然后使用 load 命令加载文件
 */

#include <iostream>
#include <iomanip>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include "core/TextProcessor.h"
#include "core/InvertedIndex.h"
#include "core/SearchEngine.h"
#include "core/Trie.h"
#include "core/Similarity.h"
#include "core/StringMatch.h"

void printSeparator(char c = '=', int width = 60) {
    std::cout << std::string(width, c) << std::endl;
}

void printHeader(const std::string& title) {
    printSeparator();
    std::cout << " " << title << std::endl;
    printSeparator();
}

void printHelp() {
    std::cout << "\nAvailable commands:\n"
              << "  load <file> [name]     - Load a text file (name is optional)\n"
              << "  list                   - List loaded books\n"
              << "  search <word>          - Search for a name/place\n"
              << "  context <n>            - Show context for result #n\n"
              << "  compare <word>         - Compare string matching algorithms\n"
              << "  and <w1> <w2>          - AND query\n"
              << "  or <w1> <w2>           - OR query\n"
              << "  not <w1> <w2>          - NOT query (w1 NOT w2)\n"
              << "  phrase <phrase>        - Phrase search\n"
              << "  near <w1> <w2> <dist>  - Proximity search\n"
              << "  spell <word>           - Spelling suggestions\n"
              << "  complete <prefix>      - Auto-complete\n"
              << "  top <n>                - Top N frequent words\n"
              << "  stats                  - Show statistics\n"
              << "  help                   - Show this help\n"
              << "  quit                   - Exit program\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout << R"(
  _   _                          ____       _   _
 | | | | __ _ _ __ _ __ _   _   |  _ \ ___ | |_| |_ ___ _ __
 | |_| |/ _` | '__| '__| | | |  | |_) / _ \| __| __/ _ \ '__|
 |  _  | (_| | |  | |  | |_| |  |  __/ (_) | |_| ||  __/ |
 |_| |_|\__,_|_|  |_|   \__, |  |_|   \___/ \__|\__\___|_|
                        |___/
        Text Retrieval System - Command Line Interface
    )" << std::endl;

    std::string dataDir = ".";
    if (argc > 1) {
        dataDir = argv[1];
    }

    // 初始化组件
    InvertedIndex index;
    Trie* trie = new Trie();
    TextProcessor processor;

    int nextDocId = 1;  // 下一个文档ID

    // 存储书籍内容和搜索结果（用于 context 命令）
    std::vector<std::vector<std::string>> bookContents;
    std::vector<std::string> bookNames;
    std::vector<Posting> lastSearchResults;
    std::string lastSearchWord;

    std::cout << "Data directory: " << dataDir << std::endl;
    std::cout << "Use 'load <filename>' to load text files.\n" << std::endl;

    // 创建搜索引擎和相似度计算器
    SearchEngine searchEngine(index);
    Similarity similarity(index);

    printHelp();

    // 主循环
    std::string line;
    std::cout << "> ";
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            std::cout << "> ";
            continue;
        }

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "quit" || cmd == "exit" || cmd == "q") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        else if (cmd == "help" || cmd == "?") {
            printHelp();
        }
        else if (cmd == "load") {
            std::string filename, bookName;
            iss >> filename;
            std::getline(iss, bookName);

            // 去除前导空格
            size_t start = bookName.find_first_not_of(' ');
            if (start != std::string::npos) {
                bookName = bookName.substr(start);
            }

            if (filename.empty()) {
                std::cout << "Usage: load <filename> [book_name]" << std::endl;
                std::cout << "Example: load data/hp1.txt Harry Potter Book 1" << std::endl;
            } else {
                // 构建完整路径
                std::string filePath = filename;
                if (filename[0] != '/' && filename.find(dataDir) != 0) {
                    filePath = dataDir + "/" + filename;
                }

                // 如果没有提供书名，使用文件名
                if (bookName.empty()) {
                    size_t lastSlash = filename.find_last_of('/');
                    if (lastSlash != std::string::npos) {
                        bookName = filename.substr(lastSlash + 1);
                    } else {
                        bookName = filename;
                    }
                    // 去除扩展名
                    size_t dotPos = bookName.find_last_of('.');
                    if (dotPos != std::string::npos) {
                        bookName = bookName.substr(0, dotPos);
                    }
                }

                try {
                    auto lines = processor.readLines(filePath);
                    if (lines.empty()) {
                        std::cout << "File is empty or cannot be read: " << filePath << std::endl;
                    } else {
                        index.addDocument(nextDocId, bookName, lines);
                        bookContents.push_back(lines);
                        bookNames.push_back(bookName);
                        std::cout << "Loaded: [" << nextDocId << "] " << bookName
                                  << " - " << lines.size() << " lines" << std::endl;
                        nextDocId++;

                        // 重建 Trie
                        delete trie;
                        trie = new Trie();
                        auto topTerms = index.getTopTerms(5000);
                        for (const auto& term : topTerms) {
                            trie->insert(term.first, term.second);
                        }

                        std::cout << "Index updated: " << index.getVocabularySize()
                                  << " unique words, " << index.getTotalTermCount()
                                  << " total terms" << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cout << "Error loading file: " << filePath << std::endl;
                    std::cout << "  " << e.what() << std::endl;
                }
            }
        }
        else if (cmd == "list") {
            if (index.getDocumentCount() == 0) {
                std::cout << "No books loaded. Use 'load <filename>' to load files." << std::endl;
            } else {
                std::cout << "Loaded books:" << std::endl;
                for (size_t i = 1; i <= index.getDocumentCount(); ++i) {
                    auto info = index.getDocumentInfo(i);
                    std::cout << "  [" << i << "] " << info.name
                              << " - " << info.totalLines << " lines, "
                              << info.totalWords << " words" << std::endl;
                }
            }
        }
        else if (cmd == "search") {
            std::string word;
            iss >> word;
            if (word.empty()) {
                std::cout << "Usage: search <word>" << std::endl;
            } else {
                lastSearchResults = index.search(word);
                lastSearchWord = word;
                std::cout << "\nFound " << lastSearchResults.size() << " occurrences of '"
                          << word << "':\n" << std::endl;

                if (!lastSearchResults.empty()) {
                    // 显示表头
                    std::cout << std::setw(6) << "序号" << "    "
                              << std::setw(15) << std::left << "人名/地名"
                              << std::setw(8) << std::right << "行号"
                              << std::setw(8) << "章节" << "    "
                              << "书名" << std::endl;
                    std::cout << std::string(70, '-') << std::endl;

                    // 显示结果（最多显示前20条）
                    int limit = std::min(20, static_cast<int>(lastSearchResults.size()));
                    for (int i = 0; i < limit; ++i) {
                        const auto& p = lastSearchResults[i];
                        int bookIdx = p.docId - 1;
                        std::string bookName = (bookIdx >= 0 && bookIdx < static_cast<int>(bookNames.size()))
                            ? bookNames[bookIdx] : "Unknown";

                        std::cout << std::setw(6) << (i + 1) << "    "
                                  << std::setw(15) << std::left << word
                                  << std::setw(8) << std::right << p.line
                                  << std::setw(8) << p.chapter << "    "
                                  << bookName << std::endl;
                    }

                    if (lastSearchResults.size() > 20) {
                        std::cout << "\n... and " << (lastSearchResults.size() - 20)
                                  << " more results" << std::endl;
                    }
                    std::cout << "\nUse 'context <n>' to view context for result #n" << std::endl;
                }
            }
        }
        else if (cmd == "context") {
            int n = 0;
            iss >> n;
            if (n <= 0 || n > static_cast<int>(lastSearchResults.size())) {
                if (lastSearchResults.empty()) {
                    std::cout << "No search results. Use 'search <word>' first." << std::endl;
                } else {
                    std::cout << "Invalid result number. Enter 1-" << lastSearchResults.size() << std::endl;
                }
            } else {
                const auto& p = lastSearchResults[n - 1];
                int bookIdx = p.docId - 1;

                if (bookIdx < 0 || bookIdx >= static_cast<int>(bookContents.size())) {
                    std::cout << "Error: Book content not found." << std::endl;
                } else {
                    const auto& lines = bookContents[bookIdx];
                    std::string bookName = bookNames[bookIdx];
                    int lineIdx = p.line - 1;

                    printSeparator('=');
                    std::cout << " Context for result #" << n << std::endl;
                    std::cout << " Book: " << bookName << std::endl;
                    std::cout << " Chapter: " << p.chapter << ", Line: " << p.line << std::endl;
                    std::cout << " Search term: " << lastSearchWord << std::endl;
                    printSeparator('=');

                    // 找到段落边界（空行分隔）
                    // 向前找段落开始
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

                    // 显示上一段落
                    std::cout << "\n--- Previous paragraph ---\n";
                    // 确保上一段落确实在空行分隔之前（prevEnd < currStart - 1 表示中间有空行）
                    if (prevParaStart <= prevEnd && prevEnd >= 0 && prevEnd < currStart - 1) {
                        for (int i = prevParaStart; i <= prevEnd; ++i) {
                            std::cout << lines[i] << "\n";
                        }
                    } else {
                        std::cout << "(Beginning of document)\n";
                    }

                    // 显示当前段落
                    std::cout << "\n--- Current paragraph ---\n";
                    for (int i = currStart; i <= currEnd && i < static_cast<int>(lines.size()); ++i) {
                        std::cout << lines[i] << "\n";
                    }

                    // 显示下一段落
                    std::cout << "\n--- Next paragraph ---\n";
                    if (nextStart < static_cast<int>(lines.size())) {
                        for (int i = nextStart; i <= nextEnd && i < static_cast<int>(lines.size()); ++i) {
                            std::cout << lines[i] << "\n";
                        }
                    } else {
                        std::cout << "(End of document)\n";
                    }
                    std::cout << std::endl;
                }
            }
        }
        else if (cmd == "compare") {
            std::string word;
            iss >> word;
            if (word.empty()) {
                std::cout << "Usage: compare <word>" << std::endl;
                std::cout << "Compare different string matching algorithms on loaded books." << std::endl;
            } else if (bookContents.empty()) {
                std::cout << "No books loaded. Use 'load <file>' first." << std::endl;
            } else {
                // 合并所有书籍内容为一个大文本
                std::string fullText;
                for (const auto& book : bookContents) {
                    for (const auto& line : book) {
                        fullText += line + "\n";
                    }
                }

                std::cout << "\nComparing algorithms for pattern: \"" << word << "\"" << std::endl;
                std::cout << "Text size: " << fullText.length() << " characters" << std::endl;

                StringMatch matcher;
                auto results = matcher.compareAll(fullText, word);
                std::cout << matcher.generateReport(results);
            }
        }
        else if (cmd == "and") {
            std::string w1, w2;
            iss >> w1 >> w2;
            if (w1.empty() || w2.empty()) {
                std::cout << "Usage: and <word1> <word2>" << std::endl;
            } else {
                auto results = searchEngine.searchAND(w1, w2);
                std::cout << "Found " << results.size() << " results for '"
                          << w1 << " AND " << w2 << "'" << std::endl;
            }
        }
        else if (cmd == "or") {
            std::string w1, w2;
            iss >> w1 >> w2;
            if (w1.empty() || w2.empty()) {
                std::cout << "Usage: or <word1> <word2>" << std::endl;
            } else {
                auto results = searchEngine.searchOR(w1, w2);
                std::cout << "Found " << results.size() << " results for '"
                          << w1 << " OR " << w2 << "'" << std::endl;
            }
        }
        else if (cmd == "not") {
            std::string w1, w2;
            iss >> w1 >> w2;
            if (w1.empty() || w2.empty()) {
                std::cout << "Usage: not <word1> <word2>" << std::endl;
            } else {
                auto results = searchEngine.searchNOT(w1, w2);
                std::cout << "Found " << results.size() << " results for '"
                          << w1 << " NOT " << w2 << "'" << std::endl;
            }
        }
        else if (cmd == "phrase") {
            std::string phrase;
            std::getline(iss, phrase);
            if (phrase.empty()) {
                std::cout << "Usage: phrase <phrase>" << std::endl;
            } else {
                // 去除前导空格
                size_t start = phrase.find_first_not_of(' ');
                if (start != std::string::npos) {
                    phrase = phrase.substr(start);
                }
                auto results = searchEngine.searchPhrase(phrase);
                std::cout << "Found " << results.size() << " occurrences of phrase '"
                          << phrase << "'" << std::endl;
            }
        }
        else if (cmd == "near") {
            std::string w1, w2;
            int dist;
            iss >> w1 >> w2 >> dist;
            if (w1.empty() || w2.empty()) {
                std::cout << "Usage: near <word1> <word2> <distance>" << std::endl;
            } else {
                auto results = searchEngine.searchNear(w1, w2, dist);
                std::cout << "Found " << results.size() << " results for '"
                          << w1 << " NEAR/" << dist << " " << w2 << "'" << std::endl;
            }
        }
        else if (cmd == "spell") {
            std::string word;
            iss >> word;
            if (word.empty()) {
                std::cout << "Usage: spell <misspelled_word>" << std::endl;
            } else {
                auto suggestions = similarity.spellingSuggestions(word);
                if (suggestions.empty()) {
                    std::cout << "No suggestions found for '" << word << "'" << std::endl;
                } else {
                    std::cout << "Suggestions for '" << word << "':" << std::endl;
                    for (const auto& s : suggestions) {
                        std::cout << "  - " << s << std::endl;
                    }
                }
            }
        }
        else if (cmd == "complete") {
            std::string prefix;
            iss >> prefix;
            if (prefix.empty()) {
                std::cout << "Usage: complete <prefix>" << std::endl;
            } else {
                auto completions = trie->autocomplete(prefix);
                if (completions.empty()) {
                    std::cout << "No completions for '" << prefix << "'" << std::endl;
                } else {
                    std::cout << "Completions for '" << prefix << "':" << std::endl;
                    for (const auto& c : completions) {
                        std::cout << "  - " << c << std::endl;
                    }
                }
            }
        }
        else if (cmd == "top") {
            int n = 10;
            iss >> n;
            auto topWords = index.getTopTerms(n);
            std::cout << "Top " << n << " most frequent words:" << std::endl;
            std::cout << std::setw(4) << "#" << "  "
                      << std::setw(20) << std::left << "Word"
                      << std::setw(10) << std::right << "Count" << std::endl;
            std::cout << std::string(40, '-') << std::endl;
            int rank = 1;
            for (const auto& [word, count] : topWords) {
                std::cout << std::setw(4) << rank++ << "  "
                          << std::setw(20) << std::left << word
                          << std::setw(10) << std::right << count << std::endl;
            }
        }
        else if (cmd == "stats") {
            printHeader("Statistics");
            std::cout << "Documents: " << index.getDocumentCount() << std::endl;
            std::cout << "Vocabulary: " << index.getVocabularySize() << " unique words" << std::endl;
            std::cout << "Total terms: " << index.getTotalTermCount() << std::endl;
            if (index.getDocumentCount() > 0) {
                std::cout << "\nPer-book statistics:" << std::endl;
                for (size_t i = 1; i <= index.getDocumentCount(); ++i) {
                    auto info = index.getDocumentInfo(i);
                    std::cout << "  [" << i << "] " << info.name
                              << " - " << info.totalLines << " lines, "
                              << info.totalWords << " words" << std::endl;
                }
            }
        }
        else {
            std::cout << "Unknown command: " << cmd << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }

        std::cout << "> ";
    }

    return 0;
}
