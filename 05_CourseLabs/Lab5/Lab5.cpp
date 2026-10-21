#include <iostream>
#include <string>
#include "HistoryManager.h"
int main() {
    HistoryManager manager;
 
    int n; // 初始访问次数
    std::cin >> n;
    std::string url;
    for (int i = 0; i < n; ++i) {
        std::cin >> url;
        manager.visit(url);
    }
 
    int T; // 操作次数
    std::cin >> T;
    for (int i = 0; i < T; ++i) {
        std::string op;
        std::cin >> op;
 
        if (op == "VISIT") {
            std::cin >> url;
            manager.visit(url);
        } else if (op == "CLEAR") {
            std::cin >> url;
            manager.clearHistory(url);
        } else if (op == "FAV") {
            std::cin >> url;
            manager.addToFavorites(url);
        } else if (op == "UNDO") {
            manager.undoLastVisit();
        } else if (op == "SEARCH") {
            std::string key;
            std::cin >> key;
            manager.searchByKeyword(key);
        } else if(op == "deduplicate")
        {
            manager.deduplicateFavorites(); 
        }
    }
 
    // 输出
    manager.displayRecentHistory();
    manager.displayStatistics();
    manager.displayFavoritesSorted();
 
    return 0;
}
