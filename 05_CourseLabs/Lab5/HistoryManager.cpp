#include "HistoryManager.h"

HistoryManager::HistoryManager() {
    // Initialization if needed
}

void HistoryManager::visit(const std::string& url) {
    // TODO: Add the url to recentHistory and update its count in visitCounts
    if (!visitCounts.count(url)) {
        visitCounts[url]=0;
        visitCounts[url]++;
    }else {
        visitCounts[url]++;
    }
    recentHistory.push_back(url);

    std::cout << "Visited: " << url << std::endl;
}



void HistoryManager::clearHistory(const std::string& url) {
    // TODO: Remove all instances of the url from recentHistory and its entry in visitCounts
    visitCounts.erase(url);
    auto it = std::remove(recentHistory.begin(), recentHistory.end(), url);
    recentHistory.erase(it, recentHistory.end());


    std::cout << "Cleared all records for: " << url << std::endl;
}


void HistoryManager::addToFavorites(const std::string& url) {
    // TODO: Add the url to the favorites vector
    favorites.push_back(url);

    std::cout << "Added to favorites: " << url << std::endl;
}


std::string HistoryManager::getMostVisited() const {
    // TODO: Traverse visitCounts to find and return the URL with the highest frequency
    // Return "None" if the map is empty.
    int most_visited = 0;
    std::string most_site;
    for (const auto& p : visitCounts) {
        if (most_visited<p.second) {
            most_visited=p.second;
            most_site=p.first;
        }
    }
    if (most_visited==0)
        return "None";
    return most_site;

    return ""; // Placeholder
}

void HistoryManager::deduplicateFavorites() {
    // TODO: Remove duplicate URLs from the favorites vector.
    // Hint: Use std::sort followed by std::unique and erase.
    std::sort(favorites.begin(), favorites.end());
    auto last=std::unique(favorites.begin(), favorites.end());
    favorites.erase(last, favorites.end());

    std::cout << "Favorites deduplicated.\n";
}

void HistoryManager::searchByKeyword(const std::string& key) const {
    std::cout << "--- Search results for '" << key << "' ---\n";
    // TODO: Iterate through recentHistory and print URLs that contain the keyword 'key'
    // Hint: Use std::string::find()
    for (auto& p:recentHistory) {
        auto it=p.std::string::find(key);
        if (it!=std::string::npos) {
            std::cout<<"[Match]:"<<p<<std::endl;
        }
    }
}

void HistoryManager::undoLastVisit() {
    // TODO: Remove the last visited URL from recentHistory and decrement its count in visitCounts
    // If the count reaches 0, remove the key from the map.
    if (!recentHistory.empty()) {
        std::string last_visited=recentHistory.back();
        recentHistory.pop_back();
        visitCounts[last_visited]--;
        if (visitCounts[last_visited]==0) {
            visitCounts.erase(last_visited);

        }
        std::cout<<"Undid visit to: "<<last_visited<<std::endl;
    }

}


void HistoryManager::displayStatistics() const {
    std::cout << "\n--- Statistics ---\n";
    if (visitCounts.empty()) {
        std::cout << "No statistics available.\n";
        return;
    }
    for (const auto& p : visitCounts)
        std::cout << p.first << ": " << p.second << " times\n";
}

void HistoryManager::displayRecentHistory() const {
    std::cout << "\n--- Full History (List) ---\n";
    if (recentHistory.empty()) {
        std::cout << "History is empty.\n";
        return;
    }
    for (const auto& url : recentHistory) std::cout << " > " << url << std::endl;
}

void HistoryManager::displayFavoritesSorted() const {
    std::cout << "\n--- Favorites (Sorted) ---\n";
    if (favorites.empty()) {
        std::cout << "Favorites list is empty.\n";
        return;
    }
    std::vector<std::string> temp = favorites;
    std::sort(temp.begin(), temp.end());
    for (const auto& f : temp) std::cout << " * " << f << std::endl;
}