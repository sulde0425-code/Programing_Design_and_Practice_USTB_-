/**
 * @file StringMatch.cpp
 * @brief 字符串匹配算法实现
 *
 * 本文件包含多种字符串匹配算法的实现。
 * 暴力匹配作为参考实现已提供，其他算法需要学生完成。
 */

#include "StringMatch.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

StringMatch::StringMatch() : comparisonCount_(0) {}

void StringMatch::startTimer() {
    startTime_ = std::chrono::high_resolution_clock::now();
}

double StringMatch::stopTimer() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        endTime - startTime_);
    return duration.count() / 1000.0;  // 转换为毫秒
}

void StringMatch::resetComparisons() {
    comparisonCount_ = 0;
}

void StringMatch::countComparison() {
    comparisonCount_++;
}

// ==================== 暴力匹配 ====================

/**
 * TODO: 学生实现 - 暴力匹配算法 (Brute Force)
 *
 * 算法思路：
 * 1. 从文本的每个位置开始，尝试与模式串匹配
 * 2. 如果所有字符都匹配，记录该位置
 * 3. 无论匹配成功与否，都移动到下一个位置继续尝试
 *
 * 时间复杂度：O(n*m)
 * 空间复杂度：O(1)
 */
MatchResult StringMatch::bruteForce(const std::string& text,
                                     const std::string& pattern) {
    MatchResult result;
    result.algorithm = "Brute Force";
    resetComparisons();
    startTimer();

    int n = text.length();
    int m = pattern.length();

    if (m == 0 || m > n) {
        result.comparisons = comparisonCount_;
        result.timeMs = stopTimer();
        return result;
    }

    // ===== TODO: 请在此处实现 =====
    for (int i = 0; i <= n - m; ++i) {
        int j = 0;
        while (j < m) {
            countComparison();
            if (text[i + j] != pattern[j]) {
                break;
            }
            j++;
        }
        if (j == m) {
            result.positions.push_back(i);
        }
    }
    // ===== END TODO =====

    result.comparisons = comparisonCount_;
    result.timeMs = stopTimer();
    return result;
}

// ==================== KMP 算法 ====================

/**
 * TODO: 学生实现 - 构建 KMP 失败函数
 *
 * 失败函数（也称为部分匹配表、前缀函数）的定义：
 * failure[i] = pattern[0..i] 的最长相等真前缀和真后缀的长度
 *
 * 例如，pattern = "ABABC":
 *   failure[0] = 0  ("A" 没有真前后缀)
 *   failure[1] = 0  ("AB" 的前缀{A}和后缀{B}不相等)
 *   failure[2] = 1  ("ABA" 的前缀{A}和后缀{A}相等，长度1)
 *   failure[3] = 2  ("ABAB" 的前缀{AB}和后缀{AB}相等，长度2)
 *   failure[4] = 0  ("ABABC" 没有相等的真前后缀)
 */
std::vector<int> StringMatch::buildFailureFunction(const std::string& pattern) {
    int m = pattern.length();
    std::vector<int> failure(m, 0);

    if (m == 0) return failure;

    // ===== TODO: 请在此处实现 =====
    failure[0] = 0;
    int j = 0;
    for (int i = 1; i < m; ++i) {
        while (j > 0 && pattern[i] != pattern[j]) {
            j = failure[j - 1];
        }
        if (pattern[i] == pattern[j]) {
            j++;
        }
        failure[i] = j;
    }
    // ===== END TODO =====

    return failure;
}

/**
 * TODO: 学生实现 - KMP 算法
 *
 * KMP 算法的核心思想：
 * 当发生不匹配时，利用已匹配的信息，将模式串向右滑动到合适位置，
 * 而不是简单地滑动一位。
 *
 * 时间复杂度：O(n+m)
 * 空间复杂度：O(m) - 存储失败函数
 */
MatchResult StringMatch::kmp(const std::string& text, const std::string& pattern) {
    MatchResult result;
    result.algorithm = "KMP";
    resetComparisons();
    startTimer();

    int n = text.length();
    int m = pattern.length();

    if (m == 0 || m > n) {
        result.comparisons = comparisonCount_;
        result.timeMs = stopTimer();
        return result;
    }

    // ===== TODO: 请在此处实现 =====
    std::vector<int> failure = buildFailureFunction(pattern);
    int i = 0, j = 0;
    while (i < n) {
        countComparison();
        if (text[i] == pattern[j]) {
            i++; j++;
            if (j == m) {
                result.positions.push_back(i - m);
                j = failure[j - 1];
            }
        } else if (j > 0) {
            j = failure[j - 1];
        } else {
            i++;
        }
    }
    // ===== END TODO =====

    result.comparisons = comparisonCount_;
    result.timeMs = stopTimer();
    return result;
}

// ==================== Boyer-Moore 算法 ====================

/**
 * TODO: 学生实现 - 构建坏字符表
 *
 * 坏字符表记录每个字符在模式串中最右出现的位置。
 * 如果字符不在模式串中，可以跳过整个模式串长度。
 *
 * 例如，pattern = "EXAMPLE":
 *   badChar['E'] = 6 (最右位置)
 *   badChar['X'] = 1
 *   badChar['A'] = 2
 *   badChar['M'] = 3
 *   badChar['P'] = 4
 *   badChar['L'] = 5
 *   其他字符不在表中（跳跃距离为 m）
 *
 * 实现提示：
 * 遍历模式串，记录每个字符最后出现的位置
 */
std::map<char, int> StringMatch::buildBadCharTable(const std::string& pattern) {
    std::map<char, int> badChar;

    // ===== TODO: 请在此处实现 =====
    for (int i = 0; i < static_cast<int>(pattern.length()); ++i) {
        badChar[pattern[i]] = i;
    }
    return badChar;
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - Boyer-Moore 算法主体（简化版，只用坏字符规则）
 *
 * Boyer-Moore 算法的核心思想：
 * 1. 从右向左比较模式串和文本
 * 2. 当发生不匹配时，利用坏字符规则计算跳跃距离
 *
 * 坏字符规则：
 * 当 text[i+j] != pattern[j] 时，查看 text[i+j] 在模式串中的位置 k
 * - 如果 k < j，可以将模式串右移 j - k 位
 * - 如果 k >= j 或字符不存在，右移 1 位（保守策略）或更多
 *
 * 实现提示：
 * 1. 构建坏字符表
 * 2. i 从 0 开始，每次比较从 pattern 的末尾开始
 * 3. 匹配成功记录位置，不匹配时根据坏字符规则跳跃
 */
MatchResult StringMatch::boyerMoore(const std::string& text,
                                     const std::string& pattern) {
    MatchResult result;
    result.algorithm = "Boyer-Moore";
    resetComparisons();
    startTimer();

    int n = text.length();
    int m = pattern.length();

    if (m == 0 || m > n) {
        result.comparisons = comparisonCount_;
        result.timeMs = stopTimer();
        return result;
    }

    // ===== TODO: 请在此处实现 =====
    std::map<char, int> badChar = buildBadCharTable(pattern);

    int i = 0;
    while (i <= n - m) {
        int j = m - 1;  // 从右向左比较

        while (j >= 0) {
            countComparison();
            if (text[i + j] != pattern[j]) break;
            j--;
        }

        if (j < 0) {
            // 完全匹配
            result.positions.push_back(i);
            i++;
        } else {
            // 计算跳跃距离
            char badCharacter = text[i + j];
            int shift = 1;
            if (badChar.count(badCharacter)) {
                shift = std::max(1, j - badChar[badCharacter]);
            } else {
                shift = j + 1;
            }
            i += shift;
        }
    }
    // ===== END TODO =====

    result.comparisons = comparisonCount_;
    result.timeMs = stopTimer();
    return result;
}

// ==================== Rabin-Karp 算法 ====================

/**
 * 计算字符串哈希值（框架提供）
 */
long long StringMatch::computeHash(const std::string& str, long long base,
                                    long long mod) {
    long long hash = 0;
    for (char c : str) {
        hash = (hash * base + c) % mod;
    }
    return hash;
}

/**
 * TODO: 学生实现 - Rabin-Karp 算法
 *
 * Rabin-Karp 算法的核心思想：
 * 1. 计算模式串的哈希值
 * 2. 计算文本中每个长度为 m 的子串的哈希值
 * 3. 如果哈希值相等，再逐字符验证（避免哈希冲突）
 *
 * 滚动哈希公式：
 * newHash = (oldHash - text[i] * base^(m-1)) * base + text[i+m]
 *
 * 实现提示：
 * 1. 计算模式串哈希值 patternHash
 * 2. 计算文本前 m 个字符的哈希值 textHash
 * 3. 如果 textHash == patternHash，验证是否真正匹配
 * 4. 滚动更新 textHash，继续比较
 * 5. 注意处理取模运算中的负数
 */
MatchResult StringMatch::rabinKarp(const std::string& text,
                                    const std::string& pattern) {
    MatchResult result;
    result.algorithm = "Rabin-Karp";
    resetComparisons();
    startTimer();

    int n = text.length();
    int m = pattern.length();

    if (m == 0 || m > n) {
        result.comparisons = comparisonCount_;
        result.timeMs = stopTimer();
        return result;
    }

    // ===== TODO: 请在此处实现 =====
    const long long BASE = 256;
    const long long MOD = 1000000007;

    // 计算 base^(m-1) % MOD
    long long h = 1;
    for (int i = 0; i < m - 1; ++i) {
        h = (h * BASE) % MOD;
    }

    // 计算初始哈希值
    long long patternHash = computeHash(pattern, BASE, MOD);
    long long textHash = computeHash(text.substr(0, m), BASE, MOD);

    for (int i = 0; i <= n - m; ++i) {
        countComparison();

        // 哈希值匹配，验证字符串
        if (textHash == patternHash) {
            bool match = true;
            for (int j = 0; j < m; ++j) {
                countComparison();
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                result.positions.push_back(i);
            }
        }

        // 滚动哈希：移除第一个字符，添加下一个字符
        if (i < n - m) {
            textHash = ((textHash - text[i] * h % MOD + MOD) * BASE
                        + text[i + m]) % MOD;
        }
    }
    // ===== END TODO =====

    result.comparisons = comparisonCount_;
    result.timeMs = stopTimer();
    return result;
}

// ==================== 对比功能 ====================

std::vector<MatchResult> StringMatch::compareAll(const std::string& text,
                                                  const std::string& pattern) {
    std::vector<MatchResult> results;

    results.push_back(bruteForce(text, pattern));
    results.push_back(kmp(text, pattern));
    results.push_back(boyerMoore(text, pattern));
    results.push_back(rabinKarp(text, pattern));

    return results;
}

std::string StringMatch::generateReport(const std::vector<MatchResult>& results) {
    std::ostringstream report;

    report << "\n";
    report << "╔══════════════════════════════════════════════════════════════╗\n";
    report << "║           字符串匹配算法性能对比报告                         ║\n";
    report << "╠══════════════════════════════════════════════════════════════╣\n";
    report << "║ 算法名称             │ 匹配数 │ 比较次数   │ 时间(ms)  │ 复杂度 ║\n";
    report << "╠──────────────────────┼────────┼────────────┼───────────┼────────╣\n";

    std::vector<std::string> complexities = {
        "O(nm)",      // Brute Force
        "O(n+m)",     // KMP
        "O(n/m)",     // Boyer-Moore
        "O(n+m)"      // Rabin-Karp
    };

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        std::string algoName = r.algorithm;
        if (r.positions.empty() && r.comparisons == 0) {
            algoName += " (TODO)";
        }
        report << "║ " << std::setw(20) << std::left << algoName
               << " │ " << std::setw(6) << std::right << r.positions.size()
               << " │ " << std::setw(10) << r.comparisons
               << " │ " << std::setw(9) << std::fixed << std::setprecision(3) << r.timeMs
               << " │ " << std::setw(6) << complexities[i] << " ║\n";
    }

    report << "╚══════════════════════════════════════════════════════════════╝\n";

    // 统计已实现的算法
    std::vector<MatchResult> implemented;
    for (const auto& r : results) {
        if (r.comparisons > 0 || !r.positions.empty()) {
            implemented.push_back(r);
        }
    }

    if (implemented.empty()) {
        report << "\n⚠ 所有算法均未实现！请完成 StringMatch.cpp 中的 TODO 部分。\n";
        report << "\n实现顺序建议：\n";
        report << "  1. Brute Force - 最简单，作为基准\n";
        report << "  2. KMP - 理解失败函数的构建\n";
        report << "  3. Boyer-Moore - 理解坏字符规则\n";
        report << "  4. Rabin-Karp - 理解滚动哈希\n";
    } else {
        auto fastest = std::min_element(implemented.begin(), implemented.end(),
            [](const MatchResult& a, const MatchResult& b) {
                return a.timeMs < b.timeMs;
            });
        auto fewest = std::min_element(implemented.begin(), implemented.end(),
            [](const MatchResult& a, const MatchResult& b) {
                return a.comparisons < b.comparisons;
            });

        report << "\n═══ 性能分析 ═══\n";
        report << "已实现: " << implemented.size() << "/4 个算法\n";
        report << "  • 最快算法: " << fastest->algorithm
               << " (" << std::fixed << std::setprecision(3) << fastest->timeMs << " ms)\n";
        report << "  • 最少比较: " << fewest->algorithm
               << " (" << fewest->comparisons << " 次)\n";

        // 检查结果一致性
        if (implemented.size() > 1) {
            bool allMatch = true;
            size_t expectedCount = implemented[0].positions.size();
            for (const auto& r : implemented) {
                if (r.positions.size() != expectedCount) {
                    allMatch = false;
                    break;
                }
            }
            if (allMatch) {
                report << "  • 正确性验证: ✓ 所有算法结果一致 (" << expectedCount << " 个匹配)\n";
            } else {
                report << "  • 正确性验证: ✗ 算法结果不一致，请检查实现！\n";
            }
        }

        // 提示未实现的算法
        int todoCount = 4 - implemented.size();
        if (todoCount > 0) {
            report << "\n还有 " << todoCount << " 个算法待实现\n";
        }
    }

    return report.str();
}
