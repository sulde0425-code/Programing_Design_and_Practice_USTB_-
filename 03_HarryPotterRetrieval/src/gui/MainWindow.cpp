/**
 * @file MainWindow.cpp
 * @brief 主窗口实现
 */

#include "MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    setupUI();
    setupConnections();

    // 初始化核心组件
    index_ = std::make_unique<InvertedIndex>();
    trie_ = std::make_unique<Trie>();
    stringMatch_ = std::make_unique<StringMatch>();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("Harry Potter Text Retrieval System");
    setMinimumSize(1000, 700);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // ===== 顶部工具栏 =====
    QHBoxLayout* toolbarLayout = new QHBoxLayout();

    loadButton_ = new QPushButton("Upload TXT Files", this);
    loadButton_->setMinimumWidth(100);

    statsButton_ = new QPushButton("Statistics", this);
    statsButton_->setMinimumWidth(100);
    statsButton_->setEnabled(false);

    toolbarLayout->addWidget(loadButton_);
    toolbarLayout->addWidget(statsButton_);
    toolbarLayout->addStretch();

    mainLayout->addLayout(toolbarLayout);

    // ===== 搜索栏 =====
    QGroupBox* searchGroup = new QGroupBox("Search", this);
    QHBoxLayout* searchLayout = new QHBoxLayout(searchGroup);

    searchModeCombo_ = new QComboBox(this);
    searchModeCombo_->addItems({
        "Single Word",
        "AND Query",
        "OR Query",
        "NOT Query",
        "Phrase",
        "NEAR/N",
        "Fuzzy Match"
    });
    searchModeCombo_->setMinimumWidth(120);

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText("Enter search query...");

    // 设置自动补全
    completerModel_ = new QStringListModel(this);
    completer_ = new QCompleter(completerModel_, this);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);
    searchEdit_->setCompleter(completer_);

    searchButton_ = new QPushButton("Search", this);
    searchButton_->setMinimumWidth(80);
    searchButton_->setEnabled(false);

    searchLayout->addWidget(searchModeCombo_);
    searchLayout->addWidget(searchEdit_, 1);
    searchLayout->addWidget(searchButton_);

    mainLayout->addWidget(searchGroup);

    // ===== 主内容区域 =====
    tabWidget_ = new QTabWidget(this);

    // Tab 1: 搜索结果
    QWidget* searchTab = new QWidget();
    QHBoxLayout* searchTabLayout = new QHBoxLayout(searchTab);

    QSplitter* splitter = new QSplitter(Qt::Horizontal);

    resultList_ = new QListWidget();
    resultList_->setMinimumWidth(300);

    contentBrowser_ = new QTextBrowser();
    contentBrowser_->setMinimumWidth(400);
    contentBrowser_->setFont(QFont("Courier New", 10));

    splitter->addWidget(resultList_);
    splitter->addWidget(contentBrowser_);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    searchTabLayout->addWidget(splitter);
    tabWidget_->addTab(searchTab, "Search Results");

    // Tab 2: 统计信息
    QWidget* statsTab = new QWidget();
    QVBoxLayout* statsTabLayout = new QVBoxLayout(statsTab);

    statsTable_ = new QTableWidget();
    statsTable_->setColumnCount(3);
    statsTable_->setHorizontalHeaderLabels({"Book", "Lines", "Words"});
    statsTable_->horizontalHeader()->setStretchLastSection(true);
    statsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);

    statsTabLayout->addWidget(statsTable_);
    tabWidget_->addTab(statsTab, "Statistics");

    // Tab 3: 算法对比
    QWidget* compareTab = new QWidget();
    QVBoxLayout* compareTabLayout = new QVBoxLayout(compareTab);

    // 对比控制栏
    QHBoxLayout* compareControlLayout = new QHBoxLayout();

    QLabel* compareLabel = new QLabel(
        QString::fromUtf8("Pattern:"), compareTab);
    comparePatternEdit_ = new QLineEdit(compareTab);
    comparePatternEdit_->setPlaceholderText(
        QString::fromUtf8("Enter pattern to compare, or click 'Random Pattern'"));

    compareButton_ = new QPushButton(
        QString::fromUtf8("Compare"), compareTab);
    compareButton_->setMinimumWidth(80);
    compareButton_->setEnabled(false);

    compareRandomButton_ = new QPushButton(
        QString::fromUtf8("Random Pattern"), compareTab);
    compareRandomButton_->setMinimumWidth(110);
    compareRandomButton_->setEnabled(false);

    compareControlLayout->addWidget(compareLabel);
    compareControlLayout->addWidget(comparePatternEdit_, 1);
    compareControlLayout->addWidget(compareButton_);
    compareControlLayout->addWidget(compareRandomButton_);

    compareTabLayout->addLayout(compareControlLayout);

    // 信息标签
    compareInfoLabel_ = new QLabel(compareTab);
    compareInfoLabel_->setWordWrap(true);
    compareInfoLabel_->setStyleSheet(
        "QLabel { color: #666; padding: 4px; }");
    compareTabLayout->addWidget(compareInfoLabel_);

    // 对比图表组件
    compareWidget_ = new CompareWidget(compareTab);
    compareWidget_->setMinimumHeight(400);
    compareTabLayout->addWidget(compareWidget_, 1);

    tabWidget_->addTab(compareTab, QString::fromUtf8("Compare"));

    mainLayout->addWidget(tabWidget_, 1);

    // ===== 进度条 =====
    progressBar_ = new QProgressBar(this);
    progressBar_->setVisible(false);
    mainLayout->addWidget(progressBar_);

    // ===== 状态栏 =====
    statusLabel_ = new QLabel("Ready. Click 'Upload TXT Files' to start.", this);
    statusBar()->addWidget(statusLabel_);
}

void MainWindow::setupConnections() {
    connect(loadButton_, &QPushButton::clicked,
            this, &MainWindow::onLoadBooksClicked);
    connect(statsButton_, &QPushButton::clicked,
            this, &MainWindow::onShowStatisticsClicked);
    connect(searchButton_, &QPushButton::clicked,
            this, &MainWindow::onSearchClicked);
    connect(searchEdit_, &QLineEdit::returnPressed,
            this, &MainWindow::onSearchClicked);
    connect(searchEdit_, &QLineEdit::textChanged,
            this, &MainWindow::onSearchTextChanged);
    connect(resultList_, &QListWidget::itemClicked,
            this, &MainWindow::onResultItemClicked);
    connect(searchModeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSearchModeChanged);

    // Compare 选项卡连接
    connect(compareButton_, &QPushButton::clicked,
            this, &MainWindow::onCompareClicked);
    connect(compareRandomButton_, &QPushButton::clicked,
            this, &MainWindow::onCompareClicked);
    connect(comparePatternEdit_, &QLineEdit::returnPressed,
            this, &MainWindow::onCompareClicked);
    connect(comparePatternEdit_, &QLineEdit::textChanged,
            this, &MainWindow::onCompareTextChanged);
}

void MainWindow::onLoadBooksClicked() {
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        "Upload TXT Files",
        QDir::currentPath(),
        "Text Files (*.txt);;All Files (*)");

    if (filePaths.isEmpty()) return;

    loadButton_->setEnabled(false);
    progressBar_->setVisible(true);
    progressBar_->setRange(0, filePaths.size());
    statusLabel_->setText("Uploading and indexing txt files...");

    // 每次上传都重置当前数据，避免旧数据混入
    booksLoaded_ = false;
    resultList_->clear();
    contentBrowser_->clear();
    statsTable_->setRowCount(0);
    bookContents_.clear();
    bookNames_.clear();
    completerModel_->setStringList({});
    searchButton_->setEnabled(false);
    statsButton_->setEnabled(false);
    compareButton_->setEnabled(false);
    compareRandomButton_->setEnabled(false);
    compareWidget_->clearData();
    fullTextCache_.clear();

    index_ = std::make_unique<InvertedIndex>();
    trie_ = std::make_unique<Trie>();
    stringMatch_ = std::make_unique<StringMatch>();
    searchEngine_.reset();
    similarity_.reset();

    int loadedCount = 0;

    for (int i = 0; i < filePaths.size(); ++i) {
        const QString& filePath = filePaths[i];
        QFile file(filePath);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            std::vector<std::string> lines;

            while (!in.atEnd()) {
                QString line = in.readLine();
                lines.push_back(line.toStdString());
            }

            if (!lines.empty()) {
                QFileInfo info(filePath);
                QString bookName = info.completeBaseName();
                if (bookName.isEmpty()) {
                    bookName = info.fileName();
                }

                int docId = loadedCount + 1;
                index_->addDocument(docId, bookName.toStdString(), lines);
                bookContents_.push_back(lines);
                bookNames_.append(bookName);
                loadedCount++;
            }

            file.close();
        } else {
            QMessageBox::warning(this, "Warning",
                QString("Could not open: %1").arg(filePath));
        }

        progressBar_->setValue(i + 1);
    }

    if (loadedCount > 0) {
        auto topTerms = index_->getTopTerms(5000);
        for (const auto& term : topTerms) {
            trie_->insert(term.first, term.second);
        }

        booksLoaded_ = true;
        searchEngine_ = std::make_unique<SearchEngine>(*index_);
        similarity_ = std::make_unique<Similarity>(*index_);

        searchButton_->setEnabled(true);
        statsButton_->setEnabled(true);
        compareButton_->setEnabled(true);
        compareRandomButton_->setEnabled(true);

        // 构建全文缓存用于 Compare 功能
        fullTextCache_.clear();
        for (const auto& lines : bookContents_) {
            for (const auto& line : lines) {
                fullTextCache_ += line + "\n";
            }
        }

        statusLabel_->setText(QString("Uploaded %1 txt files. Vocabulary: %2 words")
            .arg(loadedCount)
            .arg(index_->getVocabularySize()));
    } else {
        statusLabel_->setText("No valid txt files uploaded.");
    }

    progressBar_->setVisible(false);
    loadButton_->setEnabled(true);
}

void MainWindow::onSearchClicked() {
    if (!booksLoaded_) {
        QMessageBox::information(this, "Info", "Please upload txt files first.");
        return;
    }

    QString query = searchEdit_->text().trimmed();
    if (query.isEmpty()) return;

    resultList_->clear();
    contentBrowser_->clear();

    int mode = searchModeCombo_->currentIndex();
    std::string qstr = query.toStdString();
    std::vector<Posting> postings;

    // 辅助：去除字符串前后空格
    auto trimLocal = [](const std::string& s) -> std::string {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    };

    // 辅助：按第一个空格拆分为两个词（模式选择框已指定操作类型）
    auto splitTwo = [&](const std::string& s) -> std::pair<std::string, std::string> {
        size_t sp = s.find(' ');
        if (sp == std::string::npos) return {s, ""};
        return {trimLocal(s.substr(0, sp)), trimLocal(s.substr(sp + 1))};
    };

    switch (mode) {
        case 0: { // Single Word
            // 先用 executeQuery 尝试（可能包含 AND/OR 等关键字）
            auto results = searchEngine_->executeQuery(qstr);
            displaySearchResults(results);
            statusLabel_->setText(QString("Found %1 results for '%2'")
                .arg(results.size()).arg(query));
            return;
        }
        case 1: { // AND Query
            auto [w1, w2] = splitTwo(qstr);
            if (w2.empty()) {
                auto results = searchEngine_->executeQuery(qstr);
                displaySearchResults(results);
                statusLabel_->setText(QString("Found %1 results for '%2'")
                    .arg(results.size()).arg(query));
                return;
            }
            auto results = searchEngine_->searchAND(w1, w2);
            displayPostingResults(results);
            statusLabel_->setText(QString("AND: '%1' & '%2' → %3 results")
                .arg(QString::fromStdString(w1))
                .arg(QString::fromStdString(w2))
                .arg(results.size()));
            return;
        }
        case 2: { // OR Query
            auto [w1, w2] = splitTwo(qstr);
            if (w2.empty()) {
                auto results = searchEngine_->executeQuery(qstr);
                displaySearchResults(results);
                statusLabel_->setText(QString("Found %1 results for '%2'")
                    .arg(results.size()).arg(query));
                return;
            }
            auto results = searchEngine_->searchOR(w1, w2);
            displayPostingResults(results);
            statusLabel_->setText(QString("OR: '%1' | '%2' → %3 results")
                .arg(QString::fromStdString(w1))
                .arg(QString::fromStdString(w2))
                .arg(results.size()));
            return;
        }
        case 3: { // NOT Query
            auto [w1, w2] = splitTwo(qstr);
            if (w2.empty()) {
                auto results = searchEngine_->executeQuery(qstr);
                displaySearchResults(results);
                statusLabel_->setText(QString("Found %1 results for '%2'")
                    .arg(results.size()).arg(query));
                return;
            }
            auto results = searchEngine_->searchNOT(w1, w2);
            displayPostingResults(results);
            statusLabel_->setText(QString("NOT: '%1' exclude '%2' → %3 results")
                .arg(QString::fromStdString(w1))
                .arg(QString::fromStdString(w2))
                .arg(results.size()));
            return;
        }
        case 4: { // Phrase
            auto results = searchEngine_->searchPhrase(qstr);
            displayPostingResults(results);
            statusLabel_->setText(QString("Phrase '%1' → %2 results")
                .arg(query).arg(results.size()));
            return;
        }
        case 5: { // NEAR/N - 格式: word1 word2 distance
            // 解析三个参数
            std::istringstream iss(qstr);
            std::string w1, w2;
            int dist = 5;
            iss >> w1 >> w2 >> dist;
            if (w2.empty()) {
                auto results = searchEngine_->executeQuery(qstr);
                displaySearchResults(results);
                statusLabel_->setText(QString("Found %1 results for '%2'")
                    .arg(results.size()).arg(query));
                return;
            }
            auto results = searchEngine_->searchNear(w1, w2, dist);
            displayPostingResults(results);
            statusLabel_->setText(QString("NEAR/%3: '%1' ~ '%2' → %4 results")
                .arg(QString::fromStdString(w1))
                .arg(QString::fromStdString(w2))
                .arg(dist).arg(results.size()));
            return;
        }
        case 6: { // Fuzzy Match
            auto suggestions = similarity_->spellingSuggestions(qstr, 2, 10);
            if (suggestions.empty()) {
                statusLabel_->setText(QString("No suggestions for '%1'").arg(query));
                return;
            }
            postings = index_->search(suggestions[0]);
            displayPostingResults(postings);
            QStringList sl;
            for (const auto& s : suggestions) sl << QString::fromStdString(s);
            statusLabel_->setText(QString("Fuzzy: '%1' → [%2], using '%3' → %4 results")
                .arg(query).arg(sl.join(", "))
                .arg(QString::fromStdString(suggestions[0]))
                .arg(postings.size()));
            return;
        }
    }

    // 默认：回退到 executeQuery
    auto results = searchEngine_->executeQuery(qstr);
    displaySearchResults(results);
    statusLabel_->setText(QString("Found %1 results for '%2'")
        .arg(results.size()).arg(query));
}

void MainWindow::onSearchTextChanged(const QString& text) {
    if (text.length() >= 2 && booksLoaded_) {
        updateAutoComplete(text);
    }
}

void MainWindow::updateAutoComplete(const QString& prefix) {
    auto suggestions = trie_->autocomplete(prefix.toLower().toStdString(), 10);

    QStringList list;
    for (const auto& s : suggestions) {
        list << QString::fromStdString(s);
    }

    completerModel_->setStringList(list);
}

void MainWindow::onResultItemClicked(QListWidgetItem* item) {
    int index = resultList_->row(item);
    // 显示上下文（简化版）
    QString text = item->data(Qt::UserRole).toString();
    contentBrowser_->setText(text);
}

void MainWindow::onSearchModeChanged(int index) {
    switch (index) {
        case 0: // Single Word
            searchEdit_->setPlaceholderText("Enter a word (e.g., harry)");
            break;
        case 1: // AND
            searchEdit_->setPlaceholderText("Enter: word1 word2 (both must match)");
            break;
        case 2: // OR
            searchEdit_->setPlaceholderText("Enter: word1 word2 (either can match)");
            break;
        case 3: // NOT
            searchEdit_->setPlaceholderText("Enter: word1 word2 (word1 must match, word2 excluded)");
            break;
        case 4: // Phrase
            searchEdit_->setPlaceholderText("Enter phrase (e.g., deathly hallows)");
            break;
        case 5: // NEAR/N
            searchEdit_->setPlaceholderText("Enter: word1 NEAR/5 word2");
            break;
        case 6: // Fuzzy
            searchEdit_->setPlaceholderText("Enter misspelled word");
            break;
    }
}

void MainWindow::displaySearchResults(const std::vector<SearchResult>& results) {
    for (const auto& r : results) {
        QString text = QString("Book %1, Chapter %2, Line %3")
            .arg(r.docId)
            .arg(r.chapter)
            .arg(r.line);

        QListWidgetItem* item = new QListWidgetItem(text);

        // 存储上下文
        if (r.docId > 0 && r.docId <= static_cast<int>(bookContents_.size())) {
            int lineIdx = r.line - 1;
            if (lineIdx >= 0 && lineIdx < static_cast<int>(bookContents_[r.docId - 1].size())) {
                QString context;
                // 显示前后各2行
                for (int i = std::max(0, lineIdx - 2);
                     i <= std::min(static_cast<int>(bookContents_[r.docId - 1].size()) - 1, lineIdx + 2);
                     ++i) {
                    if (i == lineIdx) {
                        context += QString(">>> %1\n").arg(
                            QString::fromStdString(bookContents_[r.docId - 1][i]));
                    } else {
                        context += QString("    %1\n").arg(
                            QString::fromStdString(bookContents_[r.docId - 1][i]));
                    }
                }
                item->setData(Qt::UserRole, context);
            }
        }

        resultList_->addItem(item);
    }
}

void MainWindow::displayPostingResults(const std::vector<Posting>& postings) {
    for (const auto& p : postings) {
        QString text = QString("Book %1, Chapter %2, Line %3, Pos %4")
            .arg(p.docId)
            .arg(p.chapter)
            .arg(p.line)
            .arg(p.position);

        QListWidgetItem* item = new QListWidgetItem(text);

        if (p.docId > 0 && p.docId <= static_cast<int>(bookContents_.size())) {
            int lineIdx = p.line - 1;
            if (lineIdx >= 0 && lineIdx < static_cast<int>(bookContents_[p.docId - 1].size())) {
                QString context;
                for (int i = std::max(0, lineIdx - 2);
                     i <= std::min(static_cast<int>(bookContents_[p.docId - 1].size()) - 1, lineIdx + 2);
                     ++i) {
                    if (i == lineIdx) {
                        context += QString(">>> %1\n").arg(
                            QString::fromStdString(bookContents_[p.docId - 1][i]));
                    } else {
                        context += QString("    %1\n").arg(
                            QString::fromStdString(bookContents_[p.docId - 1][i]));
                    }
                }
                item->setData(Qt::UserRole, context);
            }
        }

        resultList_->addItem(item);
    }
}

void MainWindow::onShowStatisticsClicked() {
    tabWidget_->setCurrentIndex(1);
    displayStatistics();
}

void MainWindow::displayStatistics() {
    statsTable_->setRowCount(bookContents_.size());

    for (size_t i = 0; i < bookContents_.size(); ++i) {
        auto info = index_->getDocumentInfo(i + 1);

        statsTable_->setItem(i, 0,
            new QTableWidgetItem(QString::fromStdString(info.name)));
        statsTable_->setItem(i, 1,
            new QTableWidgetItem(QString::number(info.totalLines)));
        statsTable_->setItem(i, 2,
            new QTableWidgetItem(QString::number(info.totalWords)));
    }

    // 添加汇总行
    int totalLines = 0, totalWords = 0;
    for (size_t i = 0; i < bookContents_.size(); ++i) {
        auto info = index_->getDocumentInfo(i + 1);
        totalLines += info.totalLines;
        totalWords += info.totalWords;
    }

    int row = statsTable_->rowCount();
    statsTable_->setRowCount(row + 1);
    statsTable_->setItem(row, 0, new QTableWidgetItem("TOTAL"));
    statsTable_->setItem(row, 1, new QTableWidgetItem(QString::number(totalLines)));
    statsTable_->setItem(row, 2, new QTableWidgetItem(QString::number(totalWords)));

    // 高亮汇总行
    for (int col = 0; col < 3; ++col) {
        QTableWidgetItem* item = statsTable_->item(row, col);
        if (item) {
            item->setBackground(QBrush(QColor(200, 230, 255)));
            item->setFont(QFont("", -1, QFont::Bold));
        }
    }
}

// ==================== Compare 功能 ====================

void MainWindow::onCompareClicked() {
    if (!booksLoaded_ || fullTextCache_.empty()) {
        QMessageBox::information(this, "Info",
            QString::fromUtf8("Please upload txt files first."));
        return;
    }

    // 切换到 Compare 选项卡
    tabWidget_->setCurrentIndex(2);

    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    bool isRandom = (btn == compareRandomButton_);

    std::string pattern;

    if (isRandom) {
        // 随机选取模式串
        if (fullTextCache_.length() < 20) {
            QMessageBox::warning(this, "Warning",
                QString::fromUtf8("Text is too short for comparison."));
            return;
        }

        // 从文本中随机选取 5-30 字符作为模式串
        size_t maxLen = std::min<size_t>(30, fullTextCache_.length() - 1);
        size_t minLen = std::min<size_t>(5, maxLen);
        size_t patternLen = minLen + (rand() % (maxLen - minLen + 1));
        size_t startPos = rand() % (fullTextCache_.length() - patternLen);

        pattern = fullTextCache_.substr(startPos, patternLen);
        // 清理换行符和特殊字符
        std::string clean;
        for (char c : pattern) {
            if (c >= 32 && c < 127 && c != '\n' && c != '\r') {
                clean += c;
            }
        }
        if (clean.length() >= 3) {
            pattern = clean;
        }

        comparePatternEdit_->setText(QString::fromStdString(pattern));
    } else {
        // 使用用户输入的模式串
        QString qPattern = comparePatternEdit_->text().trimmed();
        if (qPattern.isEmpty()) {
            QMessageBox::information(this, "Info",
                QString::fromUtf8("Please enter a pattern string."));
            return;
        }
        pattern = qPattern.toStdString();
    }

    if (pattern.length() < 1) {
        QMessageBox::warning(this, "Warning",
            QString::fromUtf8("Pattern is too short."));
        return;
    }

    runComparison(pattern);
}

void MainWindow::onCompareTextChanged(const QString& text) {
    if (text.trimmed().isEmpty()) {
        compareButton_->setEnabled(booksLoaded_);
    } else {
        compareButton_->setEnabled(booksLoaded_);
    }
}

void MainWindow::runComparison(const std::string& pattern) {
    if (!stringMatch_) return;

    statusLabel_->setText(QString::fromUtf8("Running comparison for pattern: '%1'...")
        .arg(QString::fromStdString(pattern)));

    // ===== 1. 运行所有算法进行对比 =====
    std::vector<MatchResult> results = stringMatch_->compareAll(
        fullTextCache_, pattern);

    // 构建柱状图数据
    CompareData barData;
    barData.results = results;
    barData.pattern = pattern;
    barData.textSource = "Full Text";

    compareWidget_->setCompareData(barData);

    // ===== 2. 生成不同模式串长度的趋势数据 =====
    std::vector<TrendPoint> trendData;
    // 使用 6 个不同的模式串长度：3,5,8,12,18,25
    std::vector<int> testLengths = {3, 5, 8, 12, 18, 25};

    for (int len : testLengths) {
        if (len > static_cast<int>(fullTextCache_.length())) break;

        // 从文本中间位置取一段作为模式串
        size_t startPos = fullTextCache_.length() / 3;
        if (startPos + len > fullTextCache_.length()) {
            startPos = fullTextCache_.length() - len;
        }

        std::string subPattern = fullTextCache_.substr(startPos, len);
        // 清理
        std::string cleanPattern;
        for (char c : subPattern) {
            if (c >= 32 && c < 127 && c != '\n' && c != '\r') {
                cleanPattern += c;
            }
        }
        if (cleanPattern.length() < 3) continue;

        TrendPoint point;
        point.patternLength = static_cast<int>(cleanPattern.length());
        point.results = stringMatch_->compareAll(fullTextCache_, cleanPattern);
        trendData.push_back(point);
    }

    compareWidget_->setTrendData(trendData);

    // ===== 3. 更新信息标签 =====
    // 找出最优算法
    QString bestAlgo;
    long long bestComparisons = LLONG_MAX;
    for (const auto& r : results) {
        if (r.comparisons > 0 && r.comparisons < bestComparisons) {
            bestComparisons = r.comparisons;
            bestAlgo = QString::fromStdString(r.algorithm);
        }
    }

    QString infoText;
    infoText += QString::fromUtf8(
        "<b>Comparison Results</b> &nbsp;|&nbsp; Pattern: \"%1\" &nbsp;|&nbsp; Text length: %2 chars")
        .arg(QString::fromStdString(pattern))
        .arg(fullTextCache_.length());

    if (!bestAlgo.isEmpty()) {
        infoText += QString::fromUtf8(
            "<br><span style='color:#d32f2f;font-weight:bold;'>★ Best Algorithm: %1 (%2 comparisons)</span>")
            .arg(bestAlgo)
            .arg(bestComparisons);
    }

    compareInfoLabel_->setText(infoText);

    statusLabel_->setText(QString::fromUtf8(
        "Comparison complete: %1 algorithms compared. Best: %2")
        .arg(results.size())
        .arg(bestAlgo));
}

std::string MainWindow::pickSampleText(int startPos, int length) const {
    if (fullTextCache_.empty()) return "";
    if (startPos + length > static_cast<int>(fullTextCache_.length())) {
        length = static_cast<int>(fullTextCache_.length()) - startPos;
    }
    if (length <= 0) return "";
    return fullTextCache_.substr(startPos, length);
}
