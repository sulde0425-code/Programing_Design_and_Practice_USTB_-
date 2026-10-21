/**
 * @file InvertedIndex.cpp
 * @brief 倒排索引实现
 *
 * 本文件包含学生需要实现的核心函数。
 * 标记为 TODO 的部分需要学生完成实现。
 */

#include "InvertedIndex.h"
#include <algorithm>

InvertedIndex::InvertedIndex() {}

/**
 * TODO: 学生实现 - 添加文档到索引
 *
 * 功能：将文档内容添加到倒排索引中
 *
 * 实现提示：
 * 1. 遍历文档的每一行 (lines)
 * 2. 对每一行使用 processor_.process(line) 进行预处理，得到单词列表
 * 3. 对每个单词，创建 Posting 记录：
 *    - posting.docId = docId
 *    - posting.chapter = 当前章节号
 *    - posting.line = 行号
 *    - posting.position = 单词在行内的位置
 * 4. 将 Posting 添加到 index_[word] 向量中
 * 5. 更新 termFrequency_[word]++
 * 6. 更新 totalTermCount_ 和 info.totalWords
 *
 * 数据结构说明：
 *   index_: map<string, vector<Posting>>  // 单词 -> 出现位置列表
 *   termFrequency_: map<string, int>      // 单词 -> 总出现次数
 *   documents_: map<int, DocumentInfo>    // 文档ID -> 文档信息
 */
void InvertedIndex::addDocument(int docId, const std::string& docName,
                                 const std::vector<std::string>& lines) {
    DocumentInfo info;
    info.name = docName;
    info.totalLines = lines.size();
    info.totalWords = 0;

    int currentChapter = 0;

    for (int lineNum = 0; lineNum < static_cast<int>(lines.size()); ++lineNum) {
        const std::string& line = lines[lineNum];

        // 检测章节标记（框架提供，无需修改）
        std::string lowerLine = processor_.toLowerCase(line);
        if (lowerLine.find("chapter") == 0 && line.length() < 30) {
            currentChapter++;
            continue;
        }

        // ===== TODO: 学生实现以下部分 =====
        // 提示：
        // 1. 使用 processor_.process(line) 预处理文本，得到单词向量
        // 2. 遍历单词向量，为每个单词创建 Posting
        // 3. 将 Posting 添加到 index_[word] 中
        // 4. 更新 termFrequency_[word]
        // 5. 更新 info.totalWords 和 totalTermCount_

        // 示例代码结构（请补充完整）：
         std::vector<std::string> tokens = processor_.process(line);
         for (int pos = 0; pos < tokens.size(); ++pos) {
             const std::string& word = tokens[pos];
             Posting posting;
             posting.docId = docId;
             posting.chapter = currentChapter;
             posting.line = lineNum + 1;
             posting.position = pos;
             index_[word].push_back(posting);
             termFrequency_[word]++;
             info.totalWords++;
             totalTermCount_++;
         }

        // ===== END TODO =====
    }

    documents_[docId] = info;
}

/**
 * 搜索单词（框架提供，无需修改）
 */
std::vector<Posting> InvertedIndex::search(const std::string& word) const {
    std::string normalizedWord = processor_.toLowerCase(
        processor_.removePunctuation(word));

    auto it = index_.find(normalizedWord);
    if (it != index_.end()) {
        return it->second;
    }
    return {};
}

/**
 * TODO: 学生实现 - 获取文档频率 (DF)
 *
 * 功能：返回包含该单词的文档数量
 *
 * 实现提示：
 * 1. 使用 search(word) 获取该单词的所有 Posting
 * 2. 统计不重复的 docId 数量
 * 3. 可以使用 std::set<int> 来去重
 *
 * @param word 要查询的单词
 * @return 包含该单词的文档数量
 */
int InvertedIndex::getDocumentFrequency(const std::string& word) const {
    // ===== TODO: 请在此处实现 =====
    std::vector<Posting> postings = search(word);

    // 使用 set 统计不重复的 docId
    std::set<int> uniqueDocs;
    for (const auto& p : postings) {
        uniqueDocs.insert(p.docId);
    }

    return static_cast<int>(uniqueDocs.size());
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 获取词频 (TF)
 *
 * 功能：返回单词在指定文档中的出现次数
 *
 * 实现提示：
 * 1. 使用 search(word) 获取该单词的所有 Posting
 * 2. 遍历 Posting，统计 docId 匹配的数量
 *
 * @param word 要查询的单词
 * @param docId 文档ID
 * @return 单词在该文档中的出现次数
 */
int InvertedIndex::getTermFrequency(const std::string& word, int docId) const {
    // ===== TODO: 请在此处实现 =====
    std::vector<Posting> postings = search(word);

    int count = 0;
    for (const auto& p : postings) {
        if (p.docId == docId) {
            count++;
        }
    }

    return count;
    // ===== END TODO =====
}

// ==================== 框架提供的辅助函数（无需修改）====================

size_t InvertedIndex::getVocabularySize() const {
    return index_.size();
}

size_t InvertedIndex::getTotalTermCount() const {
    return totalTermCount_;
}

size_t InvertedIndex::getDocumentCount() const {
    return documents_.size();
}

std::set<std::string> InvertedIndex::getVocabulary() const {
    std::set<std::string> vocab;
    for (const auto& pair : index_) {
        vocab.insert(pair.first);
    }
    return vocab;
}

DocumentInfo InvertedIndex::getDocumentInfo(int docId) const {
    auto it = documents_.find(docId);
    if (it != documents_.end()) {
        return it->second;
    }
    return {"", 0, 0};
}

/**
 * TODO: 学生实现 - 获取词频最高的N个词
 *
 * 功能：返回出现频率最高的前 n 个单词及其频率
 *
 * 实现提示：
 * 1. 将 termFrequency_ 的内容复制到 vector<pair<string, int>>
 * 2. 使用 std::sort 按词频降序排序
 * 3. 返回前 n 个元素
 *
 * 参考排序写法：
 *   std::sort(terms.begin(), terms.end(),
 *       [](const auto& a, const auto& b) {
 *           return a.second > b.second;  // 按频率降序
 *       });
 *
 * @param n 返回的单词数量
 * @return 词频最高的 n 个单词及其频率
 */
std::vector<std::pair<std::string, int>> InvertedIndex::getTopTerms(int n) const {
    // ===== TODO: 请在此处实现 =====
    // 将 termFrequency_ 复制到 vector 中
    std::vector<std::pair<std::string, int>> terms(
        termFrequency_.begin(), termFrequency_.end());

    // 按频率降序排序
    std::sort(terms.begin(), terms.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

    // 返回前 n 个
    if (n < static_cast<int>(terms.size())) {
        terms.resize(n);
    }

    return terms;
    // ===== END TODO =====
}
