/**
 * @file InvertedIndex.h
 * @brief 倒排索引模块
 *
 * 训练要点：
 * 1. STL 容器运用：map, vector, unordered_map
 * 2. 倒排索引数据结构设计
 * 3. 索引构建与查询算法
 */

#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include "TextProcessor.h"

/**
 * @struct Posting
 * @brief 倒排列表中的一条记录
 */
struct Posting {
    int docId;              // 文档ID（书籍编号）
    int chapter;            // 章节号
    int line;               // 行号
    int position;           // 词在行中的位置

    bool operator<(const Posting& other) const {
        if (docId != other.docId) return docId < other.docId;
        if (chapter != other.chapter) return chapter < other.chapter;
        if (line != other.line) return line < other.line;
        return position < other.position;
    }
};

/**
 * @struct DocumentInfo
 * @brief 文档信息
 */
struct DocumentInfo {
    std::string name;       // 文档名称
    int totalWords;         // 总词数
    int totalLines;         // 总行数
};

/**
 * @class InvertedIndex
 * @brief 倒排索引
 */
class InvertedIndex {
public:
    InvertedIndex();

    /**
     * @brief 添加文档到索引
     * @param docId 文档ID
     * @param docName 文档名称
     * @param lines 文档各行内容
     *
     * TODO: 学生实现核心逻辑
     */
    void addDocument(int docId, const std::string& docName,
                     const std::vector<std::string>& lines);

    /**
     * @brief 查询单词
     * @param word 要查询的单词
     * @return 包含该单词的所有位置
     */
    std::vector<Posting> search(const std::string& word) const;

    /**
     * @brief 获取单词的文档频率 (DF)
     * @param word 单词
     * @return 包含该单词的文档数
     *
     * TODO: 学生实现
     */
    int getDocumentFrequency(const std::string& word) const;

    /**
     * @brief 获取单词的词频 (TF)
     * @param word 单词
     * @param docId 文档ID
     * @return 该单词在指定文档中的出现次数
     *
     * TODO: 学生实现
     */
    int getTermFrequency(const std::string& word, int docId) const;

    /**
     * @brief 获取词汇表大小
     * @return 不重复单词数量
     */
    size_t getVocabularySize() const;

    /**
     * @brief 获取总词数
     * @return 所有文档的总词数
     */
    size_t getTotalTermCount() const;

    /**
     * @brief 获取文档数量
     * @return 已索引的文档数
     */
    size_t getDocumentCount() const;

    /**
     * @brief 获取词频最高的N个词
     * @param n 数量
     * @return (词, 频率) 对的向量
     *
     * TODO: 学生实现
     */
    std::vector<std::pair<std::string, int>> getTopTerms(int n) const;

    /**
     * @brief 获取词汇表（所有单词）
     * @return 所有单词的集合
     */
    std::set<std::string> getVocabulary() const;

    /**
     * @brief 获取文档信息
     * @param docId 文档ID
     * @return 文档信息
     */
    DocumentInfo getDocumentInfo(int docId) const;

private:
    // 倒排索引：词 -> 位置列表
    std::map<std::string, std::vector<Posting>> index_;

    // 词频统计：词 -> 总出现次数
    std::unordered_map<std::string, int> termFrequency_;

    // 文档信息
    std::map<int, DocumentInfo> documents_;

    // 文本处理器
    TextProcessor processor_;

    // 总词数
    size_t totalTermCount_ = 0;
};

#endif // INVERTEDINDEX_H
