/**
 * @file StringMatch.h
 * @brief 字符串匹配算法模块
 *
 * 本模块包含多种经典字符串匹配算法，供学生学习和对比：
 * 1. 暴力匹配 (Brute Force) - O(nm)
 * 2. KMP 算法 (Knuth-Morris-Pratt) - O(n+m)
 * 3. Boyer-Moore 算法 - O(n/m) 最优，O(nm) 最坏
 * 4. Rabin-Karp 算法 - O(n+m) 平均
 *
 * 训练要点：
 * - 理解不同算法的时间复杂度
 * - 理解预处理的作用（失败函数、坏字符表等）
 * - 通过实际运行对比性能差异
 */

#ifndef STRINGMATCH_H
#define STRINGMATCH_H

#include <string>
#include <vector>
#include <map>
#include <chrono>

/**
 * @struct MatchResult
 * @brief 匹配结果，包含位置和性能数据
 */
struct MatchResult {
    std::vector<int> positions;     // 所有匹配位置
    int comparisons;                // 字符比较次数
    double timeMs;                  // 执行时间（毫秒）
    std::string algorithm;          // 算法名称
};

/**
 * @class StringMatch
 * @brief 字符串匹配算法集合
 */
class StringMatch {
public:
    StringMatch();

    // ==================== 算法实现 ====================

    /**
     * @brief 暴力匹配算法 (Brute Force)
     *
     * 最简单的字符串匹配算法，逐个位置尝试匹配。
     *
     * 时间复杂度：O(n*m)，其中 n 为文本长度，m 为模式长度
     * 空间复杂度：O(1)
     *
     * @param text 文本串
     * @param pattern 模式串
     * @return 匹配结果
     */
    MatchResult bruteForce(const std::string& text, const std::string& pattern);

    /**
     * @brief KMP 算法 (Knuth-Morris-Pratt)
     *
     * 利用已匹配的信息，避免重复比较。
     * 核心：构建失败函数（部分匹配表）
     *
     * 时间复杂度：O(n+m)
     * 空间复杂度：O(m) - 存储失败函数
     *
     * TODO: 学生实现
     *
     * @param text 文本串
     * @param pattern 模式串
     * @return 匹配结果
     */
    MatchResult kmp(const std::string& text, const std::string& pattern);

    /**
     * @brief Boyer-Moore 算法
     *
     * 从右向左匹配，利用坏字符规则和好后缀规则跳跃。
     * 在实际应用中通常是最快的算法。
     *
     * 时间复杂度：O(n/m) 最优（大字母表），O(nm) 最坏
     * 空间复杂度：O(m + σ)，σ 为字母表大小
     *
     * TODO: 学生实现
     *
     * @param text 文本串
     * @param pattern 模式串
     * @return 匹配结果
     */
    MatchResult boyerMoore(const std::string& text, const std::string& pattern);

    /**
     * @brief Rabin-Karp 算法
     *
     * 利用哈希函数快速比较，适合多模式匹配。
     * 使用滚动哈希避免重复计算。
     *
     * 时间复杂度：O(n+m) 平均，O(nm) 最坏（哈希冲突）
     * 空间复杂度：O(1)
     *
     * TODO: 学生实现
     *
     * @param text 文本串
     * @param pattern 模式串
     * @return 匹配结果
     */
    MatchResult rabinKarp(const std::string& text, const std::string& pattern);

    // ==================== 辅助函数 ====================

    /**
     * @brief 构建 KMP 失败函数（部分匹配表）
     *
     * 失败函数 failure[i] 表示 pattern[0..i] 的最长相等前后缀长度
     *
     * TODO: 学生实现
     *
     * @param pattern 模式串
     * @return 失败函数数组
     */
    std::vector<int> buildFailureFunction(const std::string& pattern);

    /**
     * @brief 构建 Boyer-Moore 坏字符表
     *
     * 记录每个字符在模式串中最后出现的位置
     *
     * TODO: 学生实现
     *
     * @param pattern 模式串
     * @return 坏字符表
     */
    std::map<char, int> buildBadCharTable(const std::string& pattern);

    /**
     * @brief 计算字符串哈希值（用于 Rabin-Karp）
     *
     * 使用多项式哈希：hash = s[0]*base^(m-1) + s[1]*base^(m-2) + ... + s[m-1]
     *
     * @param str 字符串
     * @param base 基数（通常用 256 或质数）
     * @param mod 模数（大质数，避免溢出）
     * @return 哈希值
     */
    long long computeHash(const std::string& str, long long base = 256,
                          long long mod = 1000000007);

    // ==================== 对比功能 ====================

    /**
     * @brief 运行所有算法并对比性能
     *
     * @param text 文本串
     * @param pattern 模式串
     * @return 各算法的结果列表
     */
    std::vector<MatchResult> compareAll(const std::string& text,
                                         const std::string& pattern);

    /**
     * @brief 生成性能对比报告
     *
     * @param results 各算法结果
     * @return 格式化的报告字符串
     */
    std::string generateReport(const std::vector<MatchResult>& results);

private:
    // 计时辅助
    std::chrono::high_resolution_clock::time_point startTime_;
    int comparisonCount_;

    void startTimer();
    double stopTimer();
    void resetComparisons();
    void countComparison();
};

#endif // STRINGMATCH_H
