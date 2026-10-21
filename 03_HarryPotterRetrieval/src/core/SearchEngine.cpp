/**
 * @file SearchEngine.cpp
 * @brief 检索引擎实现
 *
 * 本文件包含学生需要实现的核心搜索算法。
 * 标记为 TODO 的函数需要学生完成实现。
 */

#include "SearchEngine.h"
#include <sstream>
#include <algorithm>
#include <cmath>

SearchEngine::SearchEngine(InvertedIndex& index) : index_(index) {}

/**
 * 辅助函数：从 Posting 列表中提取文档ID集合（框架提供，无需修改）
 */
std::set<int> SearchEngine::getDocIds(const std::vector<Posting>& postings) {
    std::set<int> docs;
    for (const auto& p : postings) {
        docs.insert(p.docId);
    }
    return docs;
}

/**
 * TODO: 学生实现 - AND 查询（布尔交集）
 *
 * 功能：返回同时包含 word1 和 word2 的文档中的匹配位置
 *
 * 实现提示：
 * 1. 使用 index_.search(word1) 获取 word1 的 Posting 列表
 * 2. 使用 index_.search(word2) 获取 word2 的 Posting 列表
 * 3. 使用 getDocIds() 获取两个词各自出现的文档集合
 * 4. 求两个集合的交集（同时出现的文档）
 * 5. 返回在交集文档中的 Posting
 *
 * 示例：
 *   "harry AND hermione" 返回同时包含两个词的文档位置
 *
 * @param word1 第一个搜索词
 * @param word2 第二个搜索词
 * @return 满足条件的 Posting 列表
 */
std::vector<Posting> SearchEngine::searchAND(const std::string& word1,
                                              const std::string& word2) {
    // ===== TODO: 请在此处实现 =====
    std::vector<Posting> postings1 = index_.search(word1);
    std::vector<Posting> postings2 = index_.search(word2);

    // 获取两个词各自出现的文档集合
    std::set<int> docs1 = getDocIds(postings1);
    std::set<int> docs2 = getDocIds(postings2);

    // 求文档交集
    std::set<int> commonDocs;
    for (int docId : docs1) {
        if (docs2.count(docId)) {
            commonDocs.insert(docId);
        }
    }

    // 返回在交集文档中的 Posting（合并两个词的 posting）
    std::set<Posting> resultSet;
    for (const auto& p : postings1) {
        if (commonDocs.count(p.docId)) {
            resultSet.insert(p);
        }
    }
    for (const auto& p : postings2) {
        if (commonDocs.count(p.docId)) {
            resultSet.insert(p);
        }
    }

    return std::vector<Posting>(resultSet.begin(), resultSet.end());
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - OR 查询（布尔并集）
 *
 * 功能：返回包含 word1 或 word2（或两者）的文档中的匹配位置
 *
 * 实现提示：
 * 1. 获取两个词的 Posting 列表
 * 2. 合并两个列表
 * 3. 可以使用 std::set<Posting> 来去重
 *
 * @param word1 第一个搜索词
 * @param word2 第二个搜索词
 * @return 满足条件的 Posting 列表
 */
std::vector<Posting> SearchEngine::searchOR(const std::string& word1,
                                             const std::string& word2) {
    // ===== TODO: 请在此处实现 =====
    std::vector<Posting> postings1 = index_.search(word1);
    std::vector<Posting> postings2 = index_.search(word2);

    // 使用 set 合并去重
    std::set<Posting> resultSet;
    for (const auto& p : postings1) {
        resultSet.insert(p);
    }
    for (const auto& p : postings2) {
        resultSet.insert(p);
    }

    return std::vector<Posting>(resultSet.begin(), resultSet.end());
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - NOT 查询（布尔差集）
 *
 * 功能：返回包含 word1 但不包含 word2 的文档中的匹配位置
 *
 * 实现提示：
 * 1. 获取 word1 的 Posting 列表
 * 2. 获取 word2 出现的文档集合
 * 3. 从 word1 的结果中排除在 word2 文档中出现的 Posting
 *
 * @param word1 必须包含的词
 * @param word2 必须排除的词
 * @return 满足条件的 Posting 列表
 */
std::vector<Posting> SearchEngine::searchNOT(const std::string& word1,
                                              const std::string& word2) {
    // ===== TODO: 请在此处实现 =====
    std::vector<Posting> postings1 = index_.search(word1);
    std::vector<Posting> postings2 = index_.search(word2);

    // 获取 word2 出现的文档集合
    std::set<int> excludeDocs = getDocIds(postings2);

    // 排除出现在 word2 文档中的 Posting
    std::vector<Posting> result;
    for (const auto& p : postings1) {
        if (!excludeDocs.count(p.docId)) {
            result.push_back(p);
        }
    }

    return result;
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 短语检索
 *
 * 功能：查找短语在文档中的精确匹配位置
 *
 * 实现提示：
 * 1. 将短语分词，得到单词列表 [w1, w2, w3, ...]
 * 2. 获取第一个词 w1 的所有位置
 * 3. 对于 w1 的每个位置 p，检查：
 *    - w2 是否出现在同一行的 position = p+1
 *    - w3 是否出现在同一行的 position = p+2
 *    - ...依此类推
 * 4. 如果所有词都连续匹配，记录这个位置
 *
 * 示例："deathly hallows"
 * - 找到 "deathly" 的所有位置
 * - 对于每个位置 p，检查 "hallows" 是否出现在 p+1
 *
 * @param phrase 要搜索的短语
 * @return 匹配位置的 Posting 列表
 */
std::vector<Posting> SearchEngine::searchPhrase(const std::string& phrase) {
    // ===== TODO: 请在此处实现 =====
    // 分词
    std::istringstream stream(phrase);
    std::vector<std::string> words;
    std::string word;
    while (stream >> word) {
        words.push_back(word);
    }

    if (words.empty()) {
        return {};
    }

    // 获取第一个词的所有 posting
    std::vector<Posting> firstPostings = index_.search(words[0]);
    std::vector<Posting> result;

    // 对第一个词的每个位置，检查后续词是否连续出现
    for (const auto& p : firstPostings) {
        bool match = true;
        for (size_t i = 1; i < words.size(); ++i) {
            std::vector<Posting> nextPostings = index_.search(words[i]);
            bool found = false;
            for (const auto& np : nextPostings) {
                // 同一文档、同一章节、同一行、位置连续
                if (np.docId == p.docId &&
                    np.chapter == p.chapter &&
                    np.line == p.line &&
                    np.position == p.position + static_cast<int>(i)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                match = false;
                break;
            }
        }
        if (match) {
            result.push_back(p);
        }
    }

    return result;
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 邻近检索
 *
 * 功能：查找两个词在 distance 个单词范围内出现的位置
 *
 * 实现提示：
 * 1. 获取两个词的 Posting 列表
 * 2. 对于同一文档同一行的 Posting 对 (p1, p2)
 * 3. 计算位置差：|p1.position - p2.position|
 * 4. 如果位置差 <= distance 且 > 0，记录结果
 *
 * 示例："harry NEAR/5 wand"
 * - 找到 harry 和 wand 在同一行出现的位置
 * - 检查位置差是否 <= 5
 *
 * @param word1 第一个搜索词
 * @param word2 第二个搜索词
 * @param distance 最大距离
 * @return 满足条件的 Posting 列表
 */
std::vector<Posting> SearchEngine::searchNear(const std::string& word1,
                                               const std::string& word2,
                                               int distance) {
    // ===== TODO: 请在此处实现 =====
    std::vector<Posting> postings1 = index_.search(word1);
    std::vector<Posting> postings2 = index_.search(word2);

    std::set<Posting> resultSet;

    // 对同一文档、同一章节、同一行中的 posting 对，检查位置差
    for (const auto& p1 : postings1) {
        for (const auto& p2 : postings2) {
            if (p1.docId == p2.docId &&
                p1.chapter == p2.chapter &&
                p1.line == p2.line) {
                int posDiff = std::abs(p1.position - p2.position);
                if (posDiff > 0 && posDiff <= distance) {
                    resultSet.insert(p1);
                    resultSet.insert(p2);
                }
            }
        }
    }

    return std::vector<Posting>(resultSet.begin(), resultSet.end());
    // ===== END TODO =====
}

// ==================== 框架提供的查询执行器（无需修改）====================

// 辅助：去除字符串前后空格
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<SearchResult> SearchEngine::executeQuery(const std::string& query) {
    std::vector<SearchResult> results;

    // 简单的查询解析：检测 AND, OR, NOT, NEAR
    std::string upperQuery = query;
    std::transform(upperQuery.begin(), upperQuery.end(),
                   upperQuery.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    std::vector<Posting> postings;

    if (upperQuery.find(" AND ") != std::string::npos) {
        size_t pos = upperQuery.find(" AND ");
        std::string word1 = trim(query.substr(0, pos));
        std::string word2 = trim(query.substr(pos + 5));
        postings = searchAND(word1, word2);
    } else if (upperQuery.find(" OR ") != std::string::npos) {
        size_t pos = upperQuery.find(" OR ");
        std::string word1 = trim(query.substr(0, pos));
        std::string word2 = trim(query.substr(pos + 4));
        postings = searchOR(word1, word2);
    } else if (upperQuery.find(" NOT ") != std::string::npos) {
        size_t pos = upperQuery.find(" NOT ");
        std::string word1 = trim(query.substr(0, pos));
        std::string word2 = trim(query.substr(pos + 5));
        postings = searchNOT(word1, word2);
    } else if (upperQuery.find(" NEAR/") != std::string::npos) {
        size_t pos = upperQuery.find(" NEAR/");
        std::string word1 = trim(query.substr(0, pos));
        size_t numStart = pos + 6;
        size_t numEnd = query.find(' ', numStart);
        int dist = std::stoi(query.substr(numStart, numEnd - numStart));
        std::string word2 = trim(query.substr(numEnd + 1));
        postings = searchNear(word1, word2, dist);
    } else if (query.find(' ') != std::string::npos) {
        // 包含空格，视为短语检索
        postings = searchPhrase(trim(query));
    } else {
        // 单词检索
        postings = index_.search(trim(query));
    }

    // 转换为 SearchResult
    for (const auto& p : postings) {
        SearchResult sr;
        sr.docId = p.docId;
        sr.chapter = p.chapter;
        sr.line = p.line;
        sr.score = 1.0;
        results.push_back(sr);
    }

    return results;
}
