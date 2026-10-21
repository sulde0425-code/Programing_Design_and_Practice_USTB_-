/**
 * @file Similarity.h
 * @brief 相似度计算模块
 *
 * 训练要点：
 * 1. 编辑距离（Levenshtein Distance）- 动态规划
 * 2. TF-IDF 计算
 * 3. BM25 排序算法
 * 4. 余弦相似度
 */

#ifndef SIMILARITY_H
#define SIMILARITY_H

#include <string>
#include <vector>
#include <map>
#include <cmath>
#include "InvertedIndex.h"

/**
 * @class Similarity
 * @brief 相似度计算器
 */
class Similarity {
public:
    Similarity(InvertedIndex& index);

    /**
     * @brief 计算编辑距离（Levenshtein Distance）
     * @param s1 字符串1
     * @param s2 字符串2
     * @return 编辑距离
     *
     * TODO: 学生实现
     * 提示：使用动态规划，dp[i][j] 表示 s1[0..i] 和 s2[0..j] 的编辑距离
     */
    int editDistance(const std::string& s1, const std::string& s2);

    /**
     * @brief 拼写纠错建议
     * @param word 可能拼错的单词
     * @param maxDistance 最大编辑距离
     * @param limit 最大建议数量
     * @return 建议单词列表
     *
     * TODO: 学生实现
     */
    std::vector<std::string> spellingSuggestions(const std::string& word,
                                                  int maxDistance = 2,
                                                  int limit = 5);

    /**
     * @brief 计算 TF（词频）
     * @param word 单词
     * @param docId 文档ID
     * @return TF 值
     */
    double calculateTF(const std::string& word, int docId);

    /**
     * @brief 计算 IDF（逆文档频率）
     * @param word 单词
     * @return IDF 值
     *
     * TODO: 学生实现
     * 公式：IDF = log(N / df)，其中 N 为总文档数，df 为包含该词的文档数
     */
    double calculateIDF(const std::string& word);

    /**
     * @brief 计算 TF-IDF
     * @param word 单词
     * @param docId 文档ID
     * @return TF-IDF 值
     */
    double calculateTFIDF(const std::string& word, int docId);

    /**
     * @brief 计算 BM25 得分
     * @param query 查询词列表
     * @param docId 文档ID
     * @param k1 BM25 参数 (默认 1.2)
     * @param b BM25 参数 (默认 0.75)
     * @return BM25 得分
     *
     * TODO: 学生实现
     */
    double calculateBM25(const std::vector<std::string>& query, int docId,
                         double k1 = 1.2, double b = 0.75);

    /**
     * @brief 计算余弦相似度
     * @param vec1 向量1
     * @param vec2 向量2
     * @return 余弦相似度 [0, 1]
     *
     * TODO: 学生实现
     * 公式：cos(A,B) = (A·B) / (|A| * |B|)
     */
    double cosineSimilarity(const std::vector<double>& vec1,
                            const std::vector<double>& vec2);

    /**
     * @brief 获取文档的 TF-IDF 向量
     * @param docId 文档ID
     * @return TF-IDF 向量
     */
    std::map<std::string, double> getDocumentVector(int docId);

    /**
     * @brief 使用 BM25 对文档进行排序
     * @param query 查询
     * @return (docId, score) 列表，按得分降序
     */
    std::vector<std::pair<int, double>> rankDocuments(
        const std::vector<std::string>& query);

private:
    InvertedIndex& index_;
    double avgDocLength_ = 0;

    void calculateAvgDocLength();
};

#endif // SIMILARITY_H
