/**
 * @file Trie.h
 * @brief 前缀树（Trie）模块
 *
 * 训练要点：
 * 1. 树形数据结构的设计与实现
 * 2. 指针与动态内存管理
 * 3. 递归算法
 * 4. 自动补全功能实现
 */

#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 * @struct TrieNode
 * @brief 前缀树节点
 */
struct TrieNode {
    std::map<char, std::unique_ptr<TrieNode>> children;
    bool isEndOfWord = false;
    int frequency = 0;  // 词频，用于排序补全建议
};

/**
 * @class Trie
 * @brief 前缀树
 */
class Trie {
public:
    Trie();
    ~Trie() = default;

    /**
     * @brief 插入单词
     * @param word 要插入的单词
     *
     * TODO: 学生实现
     * 提示：遍历单词的每个字符，创建或访问对应的子节点
     */
    void insert(const std::string& word);

    /**
     * @brief 插入单词（带词频）
     * @param word 单词
     * @param freq 词频
     */
    void insert(const std::string& word, int freq);

    /**
     * @brief 查找单词是否存在
     * @param word 要查找的单词
     * @return true 如果单词存在
     *
     * TODO: 学生实现
     */
    bool search(const std::string& word) const;

    /**
     * @brief 检查是否存在以 prefix 开头的单词
     * @param prefix 前缀
     * @return true 如果存在
     *
     * TODO: 学生实现
     */
    bool startsWith(const std::string& prefix) const;

    /**
     * @brief 获取所有以 prefix 开头的单词
     * @param prefix 前缀
     * @param limit 最大返回数量
     * @return 单词列表（按词频降序排序）
     *
     * TODO: 学生实现
     */
    std::vector<std::string> autocomplete(const std::string& prefix,
                                          int limit = 10) const;

    /**
     * @brief 通配符匹配
     * @param pattern 模式串（* 匹配多字符，? 匹配单字符）
     * @return 匹配的单词列表
     *
     * TODO: 学生实现（选做）
     */
    std::vector<std::string> wildcardMatch(const std::string& pattern) const;

    /**
     * @brief 获取 Trie 中的单词总数
     */
    int getWordCount() const;

private:
    std::unique_ptr<TrieNode> root_;
    int wordCount_ = 0;

    /**
     * @brief 查找前缀对应的节点
     * @param prefix 前缀
     * @return 节点指针，如果不存在返回 nullptr
     */
    TrieNode* findNode(const std::string& prefix) const;

    /**
     * @brief 收集以某节点为根的所有单词
     * @param node 起始节点
     * @param prefix 当前前缀
     * @param results 结果容器
     */
    void collectWords(TrieNode* node, const std::string& prefix,
                      std::vector<std::pair<std::string, int>>& results) const;

    /**
     * @brief 通配符匹配辅助函数
     */
    void wildcardMatchHelper(TrieNode* node, const std::string& pattern,
                             int index, const std::string& current,
                             std::vector<std::string>& results) const;
};

#endif // TRIE_H
