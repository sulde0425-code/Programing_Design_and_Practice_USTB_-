/**
 * @file TextProcessor.cpp
 * @brief 文本预处理模块实现
 *
 * 本文件包含学生需要实现的核心函数。
 * 标记为 TODO 的函数需要学生完成实现。
 */

#include "TextProcessor.h"

TextProcessor::TextProcessor() {
    initStopWords();
}

void TextProcessor::initStopWords() {
    // 常用英文停用词表（框架提供，无需修改）
    stopWords_ = {
        "a", "an", "the", "and", "or", "but", "is", "are", "was", "were",
        "be", "been", "being", "have", "has", "had", "do", "does", "did",
        "will", "would", "could", "should", "may", "might", "must",
        "i", "you", "he", "she", "it", "we", "they", "me", "him", "her",
        "us", "them", "my", "your", "his", "its", "our", "their",
        "this", "that", "these", "those", "what", "which", "who", "whom",
        "in", "on", "at", "to", "for", "of", "with", "by", "from", "as",
        "into", "through", "during", "before", "after", "above", "below",
        "up", "down", "out", "off", "over", "under", "again", "then",
        "once", "here", "there", "when", "where", "why", "how", "all",
        "each", "few", "more", "most", "other", "some", "such", "no",
        "nor", "not", "only", "own", "same", "so", "than", "too", "very",
        "just", "can", "now", "said", "one", "two"
    };
}

// ==================== 框架提供的基础函数（无需修改）====================

std::string TextProcessor::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> TextProcessor::readLines(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool TextProcessor::isStopWord(const std::string& word) const {
    return stopWords_.find(word) != stopWords_.end();
}

// ==================== TODO: 学生需要实现的函数 ====================

/**
 * TODO: 学生实现 - 转换为小写
 *
 * 功能：将输入字符串中的所有大写字母转换为小写
 *
 * 实现提示：
 * 1. 创建结果字符串（可以复制输入字符串）
 * 2. 使用 std::transform 将每个字符转换为小写
 * 3. 返回结果
 *
 * 参考：
 *   std::transform(str.begin(), str.end(), str.begin(),
 *       [](unsigned char c) { return std::tolower(c); });
 *
 * 注意：MSVC 下必须将 char 转为 unsigned char 再传给 std::tolower，
 *       否则遇到非 ASCII 字符（如 UTF-8）会触发断言错误！
 *       错误写法：std::tolower(c)  其中 c 是 char
 *       正确写法：std::tolower(static_cast<unsigned char>(c))
 *
 * @param str 输入字符串
 * @return 转换后的小写字符串
 */
std::string TextProcessor::toLowerCase(const std::string& str) const {
    // ===== TODO: 请在此处实现 =====
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 去除标点符号
 *
 * 功能：从字符串中移除所有标点符号，只保留字母、数字和空格
 *
 * 实现提示：
 * 1. 创建空的结果字符串
 * 2. 遍历输入字符串的每个字符
 * 3. 如果字符是字母、数字或空格，则添加到结果中
 * 4. 返回结果
 *
 * 有用的函数：
 *   std::isalnum(static_cast<unsigned char>(c)) - 判断是否为字母或数字
 *   std::isspace(static_cast<unsigned char>(c)) - 判断是否为空格
 *
 * 注意：MSVC 下必须 static_cast<unsigned char> 后再调用 isalnum/isspace，
 *       否则遇到非 ASCII 字符会触发运行时断言错误！
 *
 * @param str 输入字符串
 * @return 去除标点后的字符串
 */
std::string TextProcessor::removePunctuation(const std::string& str) const {
    // ===== TODO: 请在此处实现 =====
    std::string result;
    for (char c : str) {
        if (std::isalnum(static_cast<unsigned char>(c))||std::isspace(static_cast<unsigned char>(c))) {
            result += c;
        }
    }

    return result;  // 占位返回，请修改
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 分词
 *
 * 功能：将文本按空格分割成单词列表
 *
 * 实现提示：
 * 1. 创建空的 vector<string> 存储结果
 * 2. 使用 std::istringstream 从文本创建流
 * 3. 循环读取每个单词（使用 >> 操作符）
 * 4. 将非空单词添加到结果向量
 * 5. 返回结果
 *
 * 参考：
 *   std::istringstream stream(text);
 *   std::string word;
 *   while (stream >> word) { ... }
 *
 * @param text 输入文本
 * @return 单词列表
 */
std::vector<std::string> TextProcessor::tokenize(const std::string& text) const {
    // ===== TODO: 请在此处实现 =====
    std::istringstream stream(text);
    std::string s;
    std::vector<std::string> result;
    while (stream >> s) {
        result.push_back(s);
    }

    return result;  // 占位返回空列表，请修改
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 过滤停用词
 *
 * 功能：从单词列表中移除停用词（如 the, a, is 等常见词）
 *
 * 实现提示：
 * 1. 创建空的结果向量
 * 2. 遍历输入的 tokens
 * 3. 对每个 token，使用 isStopWord() 判断是否为停用词
 * 4. 如果不是停用词，添加到结果向量
 * 5. 返回结果
 *
 * @param tokens 输入单词列表
 * @return 过滤后的单词列表
 */
std::vector<std::string> TextProcessor::removeStopWords(
    const std::vector<std::string>& tokens) {
    // ===== TODO: 请在此处实现 =====
    std::vector<std::string> result;
    for (std::string s : tokens) {
        if (!isStopWord(s)) {
            result.push_back(s);
        }
    }
    return result;  // 占位返回原列表，请修改
    // ===== END TODO =====
}

/**
 * TODO: 学生实现（选做）- 简化版词干提取
 *
 * 功能：提取单词的词干（如 running -> run）
 *
 * 实现提示（Porter Stemmer 简化版）：
 * 1. 如果单词以 "ing" 结尾且长度 > 5，去除 "ing"
 * 2. 如果单词以 "ed" 结尾且长度 > 4，去除 "ed"
 * 3. 如果单词以 "s" 结尾（但不是 "ss"）且长度 > 3，去除 "s"
 *
 * 注意：这是简化版本，完整的 Porter Stemmer 有更复杂的规则
 *
 * @param word 输入单词
 * @return 词干
 */
std::string TextProcessor::stem(const std::string& word) {
    // ===== TODO: 请在此处实现（选做）=====
    std::string result = word;
    size_t n = result.size();

    if (n > 5 && result.substr(n - 3) == "ing") {
        result.erase(n - 3);
    }
    else if (n > 4 && result.substr(n - 2) == "ed") {
        result.erase(n - 2);
    }
    else if (n > 3 && result.back() == 's') {
        if (result[n - 2] != 's') {
            result.pop_back();
        }
    }
    return result;  // 占位返回原词，请修改
    // ===== END TODO =====
}

// ==================== 整合函数（无需修改）====================

std::vector<std::string> TextProcessor::process(const std::string& text) {
    // 完整预处理流程：小写 -> 去标点 -> 分词 -> 去停用词
    std::string lower = toLowerCase(text);
    std::string cleaned = removePunctuation(lower);
    std::vector<std::string> tokens = tokenize(cleaned);
    return removeStopWords(tokens);
}
