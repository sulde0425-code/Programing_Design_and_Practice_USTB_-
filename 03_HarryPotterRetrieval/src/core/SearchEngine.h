/**
 * @file SearchEngine.h
 * @brief 检索引擎模块
 *
 * 训练要点：
 * 1. 布尔检索：AND, OR, NOT 操作
 * 2. 短语检索：连续词匹配
 * 3. 邻近检索：词间距离约束
 * 4. 集合操作：交集、并集、差集
 */

#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <string>
#include <vector>
#include <set>
#include "InvertedIndex.h"

/**
 * @struct SearchResult
 * @brief 检索结果
 */
struct SearchResult {
    int docId;
    int chapter;
    int line;
    std::string context;    // 上下文片段
    double score;           // 相关性得分

    bool operator<(const SearchResult& other) const {
        return score > other.score;  // 按得分降序
    }
};

/**
 * @class SearchEngine
 * @brief 检索引擎
 */
class SearchEngine {
public:
    SearchEngine(InvertedIndex& index);

    /**
     * @brief AND 查询
     * @param word1 第一个词
     * @param word2 第二个词
     * @return 同时包含两个词的文档位置
     *
     * TODO: 学生实现
     * 提示：求两个词的位置列表的交集
     */
    std::vector<Posting> searchAND(const std::string& word1,
                                   const std::string& word2);

    /**
     * @brief OR 查询
     * @param word1 第一个词
     * @param word2 第二个词
     * @return 包含任一词的文档位置
     *
     * TODO: 学生实现
     * 提示：求两个词的位置列表的并集
     */
    std::vector<Posting> searchOR(const std::string& word1,
                                  const std::string& word2);

    /**
     * @brief NOT 查询
     * @param word1 必须包含的词
     * @param word2 必须不包含的词
     * @return 包含 word1 但不包含 word2 的位置
     *
     * TODO: 学生实现
     * 提示：从 word1 的结果中排除 word2 出现的文档
     */
    std::vector<Posting> searchNOT(const std::string& word1,
                                   const std::string& word2);

    /**
     * @brief 短语检索
     * @param phrase 短语（空格分隔的多个词）
     * @return 包含该短语的位置
     *
     * TODO: 学生实现
     * 提示：查找词在文本中连续出现的位置
     */
    std::vector<Posting> searchPhrase(const std::string& phrase);

    /**
     * @brief 邻近检索
     * @param word1 第一个词
     * @param word2 第二个词
     * @param distance 最大距离
     * @return 两词在指定距离内共现的位置
     *
     * TODO: 学生实现
     * 提示：检查两词位置差是否在 distance 范围内
     */
    std::vector<Posting> searchNear(const std::string& word1,
                                    const std::string& word2,
                                    int distance);

    /**
     * @brief 解析并执行复合查询
     * @param query 查询字符串（如 "harry AND hermione"）
     * @return 检索结果
     */
    std::vector<SearchResult> executeQuery(const std::string& query);

private:
    InvertedIndex& index_;

    /**
     * @brief 获取某行的文档上下文
     */
    std::set<int> getDocIds(const std::vector<Posting>& postings);
};

#endif // SEARCHENGINE_H
