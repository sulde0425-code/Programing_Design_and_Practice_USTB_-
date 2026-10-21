/**
 * @file MainWindow.h
 * @brief 主窗口界面
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QListWidget>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QProgressBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QCompleter>
#include <QStringListModel>

#include "../core/InvertedIndex.h"
#include "../core/SearchEngine.h"
#include "../core/Trie.h"
#include "../core/Similarity.h"
#include "../core/StringMatch.h"
#include "CompareWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSearchClicked();
    void onSearchTextChanged(const QString& text);
    void onResultItemClicked(QListWidgetItem* item);
    void onSearchModeChanged(int index);
    void onLoadBooksClicked();
    void onShowStatisticsClicked();
    void onCompareClicked();
    void onCompareTextChanged(const QString& text);

private:
    void setupUI();
    void setupConnections();
    void loadBooks();
    void updateAutoComplete(const QString& prefix);
    void displaySearchResults(const std::vector<SearchResult>& results);
    void displayPostingResults(const std::vector<Posting>& postings);
    void displayStatistics();
    void runComparison(const std::string& pattern);
    std::string pickSampleText(int startPos, int length) const;

    // UI 组件
    QLineEdit* searchEdit_;
    QPushButton* searchButton_;
    QPushButton* loadButton_;
    QPushButton* statsButton_;
    QComboBox* searchModeCombo_;
    QListWidget* resultList_;
    QTextBrowser* contentBrowser_;
    QProgressBar* progressBar_;
    QLabel* statusLabel_;
    QTabWidget* tabWidget_;
    QTableWidget* statsTable_;
    QCompleter* completer_;
    QStringListModel* completerModel_;

    // Compare 选项卡组件
    QLineEdit* comparePatternEdit_;
    QPushButton* compareButton_;
    QPushButton* compareRandomButton_;
    CompareWidget* compareWidget_;
    QLabel* compareInfoLabel_;

    // 核心组件
    std::unique_ptr<InvertedIndex> index_;
    std::unique_ptr<SearchEngine> searchEngine_;
    std::unique_ptr<Trie> trie_;
    std::unique_ptr<Similarity> similarity_;
    std::unique_ptr<StringMatch> stringMatch_;

    // 数据
    std::vector<std::vector<std::string>> bookContents_;
    QStringList bookNames_;
    bool booksLoaded_ = false;
    std::string fullTextCache_;  // 用于 Compare 的全文缓存
};

#endif // MAINWINDOW_H
