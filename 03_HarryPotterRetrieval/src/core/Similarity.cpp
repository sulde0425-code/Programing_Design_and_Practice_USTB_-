/**
 * @file Similarity.cpp
 * @brief 相似度计算模块实现
 *
 * 本文件包含学生需要实现的相似度算法。
 * 标记为 TODO 的函数需要学生完成实现。
 */

#include "Similarity.h"
#include <algorithm>

Similarity::Similarity(InvertedIndex& index) : index_(index) {
    calculateAvgDocLength();
}

void Similarity::calculateAvgDocLength() {
    size_t totalWords = 0;
    size_t docCount = index_.getDocumentCount();

    for (size_t i = 1; i <= docCount; ++i) {
        auto info = index_.getDocumentInfo(i);
        totalWords += info.totalWords;
    }

    if (docCount > 0) {
        avgDocLength_ = static_cast<double>(totalWords) / docCount;
    }
}

/**
 * TODO: 学生实现 - 编辑距离（Levenshtein Distance）
 *
 * 功能：计算两个字符串之间的最小编辑距离
 * 编辑操作包括：插入、删除、替换（每个操作代价为1）
 *
 * 实现提示（动态规划）：
 * 1. 创建 (m+1) x (n+1) 的 DP 表，其中 m = s1.length(), n = s2.length()
 * 2. 初始化：dp[i][0] = i, dp[0][j] = j
 * 3. 状态转移：
 *    - 如果 s1[i-1] == s2[j-1]: dp[i][j] = dp[i-1][j-1]
 *    - 否则: dp[i][j] = 1 + min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1])
 * 4. 返回 dp[m][n]
 *
 * 示例：
 *   editDistance("harry", "hary") = 1  // 删除一个 r
 *   editDistance("cat", "cut") = 1     // 替换 a 为 u
 *
 * @param s1 第一个字符串
 * @param s2 第二个字符串
 * @return 编辑距离
 */
int Similarity::editDistance(const std::string& s1, const std::string& s2) {
    // ===== TODO: 请在此处实现 =====

    // 提示代码结构：
    // int m = s1.length(), n = s2.length();
    // std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    //
    // // 初始化边界
    // for (int i = 0; i <= m; ++i) dp[i][0] = i;
    // for (int j = 0; j <= n; ++j) dp[0][j] = j;
    //
    // // 填充 DP 表
    // for (int i = 1; i <= m; ++i) {
    //     for (int j = 1; j <= n; ++j) {
    //         if (s1[i-1] == s2[j-1]) {
    //             dp[i][j] = dp[i-1][j-1];
    //         } else {
    //             dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
    //         }
    //     }
    // }
    // return dp[m][n];

    return 0;  // 占位返回，请修改
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 拼写纠错建议
 *
 * 功能：为拼写错误的单词提供纠正建议
 *
 * 实现提示：
 * 1. 获取词库中的所有单词：index_.getVocabulary()
 * 2. 计算输入单词与每个词库单词的编辑距离
 * 3. 筛选编辑距离 <= maxDistance 的单词
 * 4. 按编辑距离升序排序
 * 5. 返回前 limit 个建议
 *
 * @param word 可能拼写错误的单词
 * @param maxDistance 最大编辑距离
 * @param limit 返回的最大建议数
 * @return 拼写建议列表
 */
std::vector<std::string> Similarity::spellingSuggestions(
    const std::string& word, int maxDistance, int limit) {
    // ===== TODO: 请在此处实现 =====
    (void)word;
    (void)maxDistance;
    (void)limit;

    return {};  // 占位返回空列表，请修改
    // ===== END TODO =====
}

/**
 * 计算 TF（框架提供）
 */
double Similarity::calculateTF(const std::string& word, int docId) {
    int tf = index_.getTermFrequency(word, docId);
    return tf > 0 ? 1 + std::log(tf) : 0;
}

/**
 * TODO: 学生实现 - 计算 IDF（逆文档频率）
 *
 * 功能：计算单词的 IDF 值
 *
 * 公式：IDF(t) = log(N / df(t))
 *   - N: 总文档数
 *   - df(t): 包含单词 t 的文档数
 *
 * 实现提示：
 * 1. 获取总文档数 N = index_.getDocumentCount()
 * 2. 获取文档频率 df = index_.getDocumentFrequency(word)
 * 3. 如果 df == 0，返回 0（避免除以零）
 * 4. 计算并返回 log(N / df)
 *
 * @param word 要计算 IDF 的单词
 * @return IDF 值
 */
double Similarity::calculateIDF(const std::string& word) {
    // ===== TODO: 请在此处实现 =====

    return 0.0;  // 占位返回，请修改
    // ===== END TODO =====
}

/**
 * 计算 TF-IDF（框架提供）
 */
double Similarity::calculateTFIDF(const std::string& word, int docId) {
    return calculateTF(word, docId) * calculateIDF(word);
}

/**
 * TODO: 学生实现 - 计算 BM25 得分
 *
 * 功能：计算查询在特定文档中的 BM25 得分
 *
 * 公式（对每个查询词求和）：
 *   BM25 = Σ IDF(qi) * (tf * (k1 + 1)) / (tf + k1 * (1 - b + b * dl/avgdl))
 *
 *   - tf: 词频（单词在文档中出现的次数）
 *   - dl: 文档长度
 *   - avgdl: 平均文档长度 (avgDocLength_)
 *   - k1, b: BM25 参数
 *
 * 实现提示：
 * 1. 获取文档长度 dl = index_.getDocumentInfo(docId).totalWords
 * 2. 对每个查询词 qi:
 *    a. 计算 IDF = calculateIDF(qi)
 *    b. 获取 tf = index_.getTermFrequency(qi, docId)
 *    c. 代入公式计算该词的 BM25 贡献
 * 3. 返回所有词的 BM25 之和
 *
 * @param query 查询词列表
 * @param docId 文档ID
 * @param k1 BM25 参数（默认 1.2）
 * @param b BM25 参数（默认 0.75）
 * @return BM25 得分
 */
double Similarity::calculateBM25(const std::vector<std::string>& query, int docId,
                                  double k1, double b) {
    // ===== TODO: 请在此处实现 =====
    (void)query;
    (void)docId;
    (void)k1;
    (void)b;

    return 0.0;  // 占位返回，请修改
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 计算余弦相似度
 *
 * 功能：计算两个向量的余弦相似度
 *
 * 公式：cosine(A, B) = (A · B) / (|A| * |B|)
 *   - A · B: 向量点积（对应维度相乘后求和）
 *   - |A|: 向量 A 的模（各维度平方和的平方根）
 *
 * 实现提示：
 * 1. 检查两个向量长度是否相等，不等则返回 0
 * 2. 计算点积：sum(vec1[i] * vec2[i])
 * 3. 计算各自的模：sqrt(sum(vec[i]^2))
 * 4. 返回点积 / (模1 * 模2)
 * 5. 注意处理模为 0 的情况
 *
 * @param vec1 第一个向量
 * @param vec2 第二个向量
 * @return 余弦相似度 [0, 1]
 */
double Similarity::cosineSimilarity(const std::vector<double>& vec1,
                                     const std::vector<double>& vec2) {
    // ===== TODO: 请在此处实现 =====
    (void)vec1;
    (void)vec2;

    return 0.0;  // 占位返回，请修改
    // ===== END TODO =====
}

/**
 * 获取文档的 TF-IDF 向量（框架提供）
 */
std::map<std::string, double> Similarity::getDocumentVector(int docId) {
    std::map<std::string, double> vector;
    auto vocab = index_.getVocabulary();

    for (const auto& term : vocab) {
        double tfidf = calculateTFIDF(term, docId);
        if (tfidf > 0) {
            vector[term] = tfidf;
        }
    }

    return vector;
}

/**
 * 使用 BM25 对文档进行排序（框架提供）
 */
std::vector<std::pair<int, double>> Similarity::rankDocuments(
    const std::vector<std::string>& query) {
    std::vector<std::pair<int, double>> scores;

    for (size_t i = 1; i <= index_.getDocumentCount(); ++i) {
        double score = calculateBM25(query, i);
        if (score > 0) {
            scores.push_back({i, score});
        }
    }

    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });

    return scores;
}
