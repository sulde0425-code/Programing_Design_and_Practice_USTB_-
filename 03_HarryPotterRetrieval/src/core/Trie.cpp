/**
 * @file Trie.cpp
 * @brief 前缀树（Trie）实现
 *
 * 本文件包含学生需要实现的 Trie 数据结构。
 * 标记为 TODO 的函数需要学生完成实现。
 */

#include "Trie.h"
#include <algorithm>

Trie::Trie() : root_(std::make_unique<TrieNode>()) {}

/**
 * TODO: 学生实现 - 插入单词
 *
 * 功能：将单词插入到 Trie 中
 *
 * 实现提示：
 * 1. 从根节点开始，获取指针 node = root_.get()
 * 2. 遍历单词的每个字符 c
 * 3. 如果 node->children[c] 不存在，创建新节点
 * 4. 移动到子节点：node = node->children[c].get()
 * 5. 遍历完成后，标记 node->isEndOfWord = true
 * 6. 增加 node->frequency 和 wordCount_
 *
 * 数据结构说明：
 *   node->children: map<char, unique_ptr<TrieNode>>
 *   node->isEndOfWord: bool，标记是否为单词结尾
 *   node->frequency: int，单词出现频率
 *
 * @param word 要插入的单词
 */
void Trie::insert(const std::string& word) {
    // ===== TODO: 请在此处实现 =====
    TrieNode* node = root_.get();
    for (char c : word) {
        if (node->children.find(c) == node->children.end()) {
            node->children[c] = std::make_unique<TrieNode>();
        }
        node = node->children[c].get();
    }
    if (!node->isEndOfWord) {
        wordCount_++;
    }
    node->isEndOfWord = true;
    node->frequency++;
    // ===== END TODO =====
}

/**
 * 带频率的插入（框架提供，依赖 insert 实现）
 */
void Trie::insert(const std::string& word, int freq) {
    insert(word);
    TrieNode* node = findNode(word);
    if (node) {
        node->frequency = freq;
    }
}

/**
 * TODO: 学生实现 - 查找单词
 *
 * 功能：检查单词是否存在于 Trie 中
 *
 * 实现提示：
 * 1. 使用 findNode(word) 找到单词对应的节点
 * 2. 如果节点存在且 isEndOfWord 为 true，返回 true
 * 3. 否则返回 false
 *
 * @param word 要查找的单词
 * @return true 如果单词存在
 */
bool Trie::search(const std::string& word) const {
    // ===== TODO: 请在此处实现 =====
    TrieNode* node = findNode(word);
    return node != nullptr && node->isEndOfWord;
    // ===== END TODO =====
}

/**
 * TODO: 学生实现 - 检查前缀是否存在
 *
 * 功能：检查是否有单词以给定前缀开头
 *
 * 实现提示：
 * 1. 使用 findNode(prefix) 找到前缀对应的节点
 * 2. 如果节点存在，返回 true（无需检查 isEndOfWord）
 *
 * @param prefix 要检查的前缀
 * @return true 如果存在以该前缀开头的单词
 */
bool Trie::startsWith(const std::string& prefix) const {
    // ===== TODO: 请在此处实现 =====
    return findNode(prefix) != nullptr;
    // ===== END TODO =====
}

/**
 * 辅助函数：查找前缀对应的节点（框架提供，供学生使用）
 */
TrieNode* Trie::findNode(const std::string& prefix) const {
    TrieNode* node = root_.get();

    for (char c : prefix) {
        auto it = node->children.find(c);
        if (it == node->children.end()) {
            return nullptr;
        }
        node = it->second.get();
    }

    return node;
}

/**
 * 辅助函数：收集子树中的所有单词（框架提供，供自动补全使用）
 */
void Trie::collectWords(TrieNode* node, const std::string& prefix,
                        std::vector<std::pair<std::string, int>>& results) const {
    if (!node) return;

    if (node->isEndOfWord) {
        results.push_back({prefix, node->frequency});
    }

    for (const auto& pair : node->children) {
        collectWords(pair.second.get(), prefix + pair.first, results);
    }
}

/**
 * TODO: 学生实现 - 自动补全
 *
 * 功能：返回以给定前缀开头的单词列表，按频率排序
 *
 * 实现提示：
 * 1. 使用 findNode(prefix) 找到前缀节点
 * 2. 如果节点不存在，返回空列表
 * 3. 使用 collectWords() 收集所有以该前缀开头的单词
 * 4. 按频率降序排序
 * 5. 返回前 limit 个单词
 *
 * @param prefix 前缀
 * @param limit 返回的最大单词数
 * @return 自动补全建议列表
 */
std::vector<std::string> Trie::autocomplete(const std::string& prefix, int limit) const {
    // ===== TODO: 请在此处实现 =====
    TrieNode* node = findNode(prefix);
    if (!node) return {};

    std::vector<std::pair<std::string, int>> allWords;
    collectWords(node, prefix, allWords);

    // 按频率降序排序
    std::sort(allWords.begin(), allWords.end(),
              [](const std::pair<std::string, int>& a,
                 const std::pair<std::string, int>& b) {
                  return a.second > b.second;
              });

    std::vector<std::string> result;
    int count = std::min(limit, static_cast<int>(allWords.size()));
    for (int i = 0; i < count; ++i) {
        result.push_back(allWords[i].first);
    }
    return result;
    // ===== END TODO =====
}

/**
 * TODO: 学生实现（选做）- 通配符匹配
 *
 * 功能：支持 * 和 ? 通配符的模式匹配
 * - * 可以匹配任意数量的字符（包括零个）
 * - ? 可以匹配单个字符
 *
 * 实现提示：
 * 1. 使用递归或回溯算法
 * 2. 遇到 * 时，尝试匹配 0 个或多个字符
 * 3. 遇到 ? 时，匹配任意一个字符
 * 4. 收集所有匹配的单词
 *
 * 示例：
 *   "har*" 匹配 "harry", "hard", "harmony" 等
 *   "h?rry" 匹配 "harry", "hurry" 等
 *
 * @param pattern 包含通配符的模式
 * @return 匹配的单词列表
 */
std::vector<std::string> Trie::wildcardMatch(const std::string& pattern) const {
    // ===== TODO: 请在此处实现（选做）=====
    std::vector<std::string> results;
    wildcardMatchHelper(root_.get(), pattern, 0, "", results);
    return results;
    // ===== END TODO =====
}

void Trie::wildcardMatchHelper(TrieNode* node, const std::string& pattern,
                               int index, const std::string& current,
                               std::vector<std::string>& results) const {
    if (!node) return;

    // 模式串已匹配完毕
    if (index == static_cast<int>(pattern.length())) {
        if (node->isEndOfWord) {
            results.push_back(current);
        }
        return;
    }

    char c = pattern[index];

    if (c == '*') {
        // * 匹配零个字符：跳过 *，在当前节点继续匹配剩余模式
        wildcardMatchHelper(node, pattern, index + 1, current, results);

        // * 匹配一个或多个字符：进入每个子节点，但保持 *（先消耗一个字符）
        for (const auto& pair : node->children) {
            wildcardMatchHelper(pair.second.get(), pattern, index,
                                current + pair.first, results);
        }
    } else if (c == '?') {
        // ? 匹配任意单个字符：尝试所有子节点
        for (const auto& pair : node->children) {
            wildcardMatchHelper(pair.second.get(), pattern, index + 1,
                                current + pair.first, results);
        }
    } else {
        // 普通字符：只匹配该字符对应的子节点
        auto it = node->children.find(c);
        if (it != node->children.end()) {
            wildcardMatchHelper(it->second.get(), pattern, index + 1,
                                current + c, results);
        }
    }
}

int Trie::getWordCount() const {
    return wordCount_;
}
