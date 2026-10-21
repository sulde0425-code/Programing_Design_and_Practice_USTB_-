#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>

class HistoryManager {
private:
    std::list<std::string> recentHistory;           // 存储访问轨迹
    std::unordered_map<std::string, int> visitCounts; // 统计访问频率
    std::vector<std::string> favorites;             // 收藏夹

public:
    HistoryManager();

    // 基础功能
    void visit(const std::string& url);
    void clearHistory(const std::string& url);
    void addToFavorites(const std::string& url);

    // 扩展功能
    std::string getMostVisited() const;              // Task 4: 频率最高
    void deduplicateFavorites();                     // Task 5: 收藏夹去重
    void searchByKeyword(const std::string& key) const; // Task 6: 关键词搜索
    void undoLastVisit();                            // Task 7: 撤销操作

    // 显示功能
    void displayStatistics() const;
    void displayRecentHistory() const;
    void displayFavoritesSorted() const;
};

#endif