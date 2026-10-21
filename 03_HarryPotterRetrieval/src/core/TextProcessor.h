/**
 * @file TextProcessor.h
 * @brief 文本预处理模块
 *
 * 训练要点：
 * 1. 字符串处理：大小写转换、标点去除
 * 2. 分词算法：将文本切分为单词序列
 * 3. 停用词过滤：使用 unordered_set 高效过滤
 * 4. 词干提取：Porter Stemmer 算法基础
 */

#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

/**
 * @struct WordPosition
 * @brief 单词位置信息
 */
struct WordPosition {
    int bookId;      // 书籍编号 (1-7)
    int chapter;     // 章节号
    int line;        // 行号
    int position;    // 在行中的位置
};

/**
 * @class TextProcessor
 * @brief 文本预处理器
 */
class TextProcessor {
public:
    TextProcessor();

    /**
     * @brief 读取文本文件
     * @param filename 文件路径
     * @return 文件内容
     */
    std::string readFile(const std::string& filename);

    /**
     * @brief 按行读取文件
     * @param filename 文件路径
     * @return 每行内容的向量
     */
    std::vector<std::string> readLines(const std::string& filename);

    /**
     * @brief 转换为小写
     * @param str 输入字符串
     * @return 小写字符串
     *
     * TODO: 学生实现
     * 提示：使用 std::transform 和 std::tolower
     */
    std::string toLowerCase(const std::string& str) const;

    /**
     * @brief 去除标点符号
     * @param str 输入字符串
     * @return 去除标点后的字符串
     *
     * TODO: 学生实现
     * 提示：使用 std::isalnum 判断字符是否为字母或数字
     */
    std::string removePunctuation(const std::string& str) const;

    /**
     * @brief 分词
     * @param text 输入文本
     * @return 单词向量
     *
     * TODO: 学生实现
     * 提示：使用 std::istringstream 按空格分割
     */
    std::vector<std::string> tokenize(const std::string& text) const;

    /**
     * @brief 判断是否为停用词
     * @param word 单词
     * @return true 如果是停用词
     */
    bool isStopWord(const std::string& word) const;

    /**
     * @brief 过滤停用词
     * @param tokens 单词向量
     * @return 过滤后的单词向量
     *
     * TODO: 学生实现
     * 提示：遍历 tokens，保留非停用词
     */
    std::vector<std::string> removeStopWords(const std::vector<std::string>& tokens);

    /**
     * @brief 简化版词干提取
     * @param word 单词
     * @return 词干
     *
     * TODO: 学生实现（选做）
     * 提示：实现简单规则，如去除 -ing, -ed, -s 等后缀
     */
    std::string stem(const std::string& word);

    /**
     * @brief 完整的文本预处理流程
     * @param text 原始文本
     * @return 处理后的单词向量
     */
    std::vector<std::string> process(const std::string& text);

private:
    std::unordered_set<std::string> stopWords_;

    void initStopWords();
};

#endif // TEXTPROCESSOR_H
