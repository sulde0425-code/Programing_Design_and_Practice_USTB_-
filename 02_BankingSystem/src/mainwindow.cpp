#include "mainwindow.h"
#include <QApplication>
#include <QHeaderView>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QSpinBox>
#include <QComboBox>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <iostream>

// ==================== 静态辅助函数 ====================

static double roundAmount(double amount) {
    return std::floor(amount * 100 + 0.5) / 100.0;
}

// ==================== 构造函数 ====================

MainWindow::MainWindow(const std::string& username, QWidget* parent)
    : QMainWindow(parent), username(username), currentDate(2008, 11, 1) {
    setWindowTitle(QString("银行账户管理系统 - %1").arg(QString::fromStdString(username)));
    resize(960, 680);

    // ==== 从日志文件重放历史状态 ====
    {
        std::ifstream in(getLogFilename());
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                if (line.empty()) continue;
                std::istringstream iss(line);
                char cmd;
                iss >> cmd;
                // 只重放状态变更命令
                if (cmd != 'q' && cmd != 's' && cmd != 'e') {
                    try {
                        // 内联执行（复用 executeCommand 逻辑）
                        char type;
                        int index, day;
                        double amount, credit, rate, fee;
                        std::string id, desc;
                        Account* account = nullptr;

                        switch (cmd) {
                        case 'a':
                            iss >> type >> id;
                            if (type == 's') {
                                iss >> rate;
                                account = new SavingsAccount(currentDate, id, rate);
                            } else {
                                iss >> credit >> rate >> fee;
                                account = new CreditAccount(currentDate, id, credit, rate, fee);
                            }
                            accounts.push_back(account);
                            break;
                        case 'd':
                            iss >> index >> amount;
                            std::getline(iss >> std::ws, desc);
                            accounts[index]->deposit(currentDate, amount, desc);
                            break;
                        case 'w':
                            iss >> index >> amount;
                            std::getline(iss >> std::ws, desc);
                            accounts[index]->withdraw(currentDate, amount, desc);
                            break;
                        case 'c':
                            iss >> day;
                            if (day >= 1 && day <= currentDate.getMaxDay() && day >= currentDate.getDay()) {
                                currentDate = Date(currentDate.getYear(), currentDate.getMonth(), day);
                            }
                            break;
                        case 'n':
                            if (currentDate.getMonth() == 12)
                                currentDate = Date(currentDate.getYear() + 1, 1, 1);
                            else
                                currentDate = Date(currentDate.getYear(), currentDate.getMonth() + 1, 1);
                            for (auto* acc : accounts) acc->settle(currentDate);
                            break;
                        }
                    } catch (...) {
                        // 重放时忽略异常
                    }
                }
            }
        }
    }

    // ==== 构建 UI ====
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // ===== 仪表盘 Tab =====
    QWidget* dashTab = new QWidget;
    QVBoxLayout* dashLayout = new QVBoxLayout(dashTab);

    lblCreditWarning = new QLabel;
    lblCreditWarning->setStyleSheet("color: red; font-size: 14px; font-weight: bold;");
    lblCreditWarning->setVisible(false);
    dashLayout->addWidget(lblCreditWarning);

    QGroupBox* statsBox = new QGroupBox("当月收支统计");
    QFormLayout* statsForm = new QFormLayout(statsBox);
    lblIncome = new QLabel("0.00");
    lblExpense = new QLabel("0.00");
    statsForm->addRow("总收入:", lblIncome);
    statsForm->addRow("总支出:", lblExpense);
    dashLayout->addWidget(statsBox);

    lblCurrentDate = new QLabel;
    lblCurrentDate->setText(QString("当前日期: %1-%2-%3")
        .arg(currentDate.getYear())
        .arg(currentDate.getMonth())
        .arg(currentDate.getDay()));
    dashLayout->addWidget(lblCurrentDate);
    dashLayout->addStretch();
    tabWidget->addTab(dashTab, "仪表盘");

    // ===== 操作区 Tab =====
    QWidget* opTab = new QWidget;
    QVBoxLayout* opLayout = new QVBoxLayout(opTab);

    // 开户面板
    QGroupBox* openBox = new QGroupBox("开设新账户");
    QFormLayout* openForm = new QFormLayout(openBox);
    comboAccountType = new QComboBox;
    comboAccountType->addItem("储蓄账户 (Savings)", "s");
    comboAccountType->addItem("信用卡账户 (Credit)", "c");
    editOpenId = new QLineEdit;
    editOpenRate = new QLineEdit;
    editOpenCredit = new QLineEdit;
    editOpenFee = new QLineEdit;
    editOpenCredit->setPlaceholderText("仅信用卡需要");
    editOpenFee->setPlaceholderText("仅信用卡需要");
    openForm->addRow("类型:", comboAccountType);
    openForm->addRow("账号:", editOpenId);
    openForm->addRow("利率:", editOpenRate);
    openForm->addRow("信用额度:", editOpenCredit);
    openForm->addRow("年费:", editOpenFee);

    connect(comboAccountType, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        [this]() {
            bool isCredit = (comboAccountType->currentData().toString() == "c");
            editOpenCredit->setEnabled(isCredit);
            editOpenFee->setEnabled(isCredit);
        });
    editOpenCredit->setEnabled(false);
    editOpenFee->setEnabled(false);

    QPushButton* btnOpen = new QPushButton("开户");
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::onCreateAccount);
    QHBoxLayout* openBtnLayout = new QHBoxLayout;
    openBtnLayout->addStretch();
    openBtnLayout->addWidget(btnOpen);
    openForm->addRow(openBtnLayout);
    opLayout->addWidget(openBox);

    // 存取款面板
    QGroupBox* txBox = new QGroupBox("存款 / 取款");
    QFormLayout* txForm = new QFormLayout(txBox);
    comboTxAccount = new QComboBox;
    editTxAmount = new QLineEdit;
    editTxDesc = new QLineEdit;
    txForm->addRow("账户:", comboTxAccount);
    txForm->addRow("金额:", editTxAmount);
    txForm->addRow("描述:", editTxDesc);

    QPushButton* btnDeposit = new QPushButton("存款");
    QPushButton* btnWithdraw = new QPushButton("取款");
    connect(btnDeposit, &QPushButton::clicked, this, &MainWindow::onDeposit);
    connect(btnWithdraw, &QPushButton::clicked, this, &MainWindow::onWithdraw);
    QHBoxLayout* txBtnLayout = new QHBoxLayout;
    txBtnLayout->addStretch();
    txBtnLayout->addWidget(btnDeposit);
    txBtnLayout->addWidget(btnWithdraw);
    txForm->addRow(txBtnLayout);
    opLayout->addWidget(txBox);

    // 日期跳转
    QGroupBox* dateBox = new QGroupBox("跳转到本月某日");
    QHBoxLayout* dateLayout = new QHBoxLayout(dateBox);
    spinTargetDay = new QSpinBox;
    spinTargetDay->setRange(1, 31);
    spinTargetDay->setValue(currentDate.getDay());
    btnChangeDay = new QPushButton("跳转到该日");
    connect(btnChangeDay, &QPushButton::clicked, this, &MainWindow::onChangeDay);
    dateLayout->addWidget(spinTargetDay);
    dateLayout->addWidget(btnChangeDay);
    opLayout->addWidget(dateBox);

    // 全局下一月
    QPushButton* btnNext = new QPushButton("进入下一月");
    connect(btnNext, &QPushButton::clicked, this, &MainWindow::onNextMonth);
    opLayout->addWidget(btnNext);
    opLayout->addStretch();
    tabWidget->addTab(opTab, "操作");

    // ===== 查询区 Tab =====
    QWidget* queryTab = new QWidget;
    QVBoxLayout* queryLayout = new QVBoxLayout(queryTab);

    QGroupBox* filterBox = new QGroupBox("查询条件");
    QHBoxLayout* filterLayout = new QHBoxLayout(filterBox);
    dateEditFrom = new QDateEdit(QDate(2008, 1, 1));
    dateEditTo   = new QDateEdit(QDate(2020, 12, 31));
    dateEditFrom->setDisplayFormat("yyyy-MM-dd");
    dateEditTo->setDisplayFormat("yyyy-MM-dd");
    comboQueryAccount = new QComboBox;   // 单账户筛选
    radioSortByTime   = new QRadioButton("按时间排序");
    radioSortByAmount = new QRadioButton("按金额降序");
    radioSortByTime->setChecked(true);

    filterLayout->addWidget(new QLabel("从:"));
    filterLayout->addWidget(dateEditFrom);
    filterLayout->addWidget(new QLabel("到:"));
    filterLayout->addWidget(dateEditTo);
    filterLayout->addWidget(new QLabel("账户:"));
    filterLayout->addWidget(comboQueryAccount);
    filterLayout->addWidget(radioSortByTime);
    filterLayout->addWidget(radioSortByAmount);

    QPushButton* btnQuery = new QPushButton("查询");
    connect(btnQuery, &QPushButton::clicked, this, &MainWindow::onQuery);
    filterLayout->addWidget(btnQuery);
    queryLayout->addWidget(filterBox);

    queryTable = new QTableWidget(0, 6);
    queryTable->setHorizontalHeaderLabels({"日期", "账号", "类型", "金额", "余额", "描述"});
    queryTable->horizontalHeader()->setStretchLastSection(true);
    queryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    queryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    queryLayout->addWidget(queryTable);

    // 选中账户余额显示
    lblSelectedBalance = new QLabel("当前账户余额: ￥0.00");
    lblSelectedBalance->setStyleSheet("font-size: 14px; font-weight: bold; padding: 6px;");
    queryLayout->addWidget(lblSelectedBalance);

    tabWidget->addTab(queryTab, "账单查询");

    // 初始化账户下拉框 + 刷新仪表盘
    refreshAllCombos();
    refreshDashboard();
}

MainWindow::~MainWindow() {
    for (auto* acc : accounts) delete acc;
}

// ==================== 日志文件路径 ====================

std::string MainWindow::getLogFilename() const {
    return "commands_" + username + ".txt";
}

// ==================== 日志写入 ====================

void MainWindow::logCommand(const std::string& cmd) {
    std::ofstream out(getLogFilename(), std::ios::app);
    out << cmd << std::endl;
}

// ==================== 刷新账户下拉框 ====================

void MainWindow::refreshAccountCombo(QComboBox* combo) {
    combo->clear();
    // 首项固定为 [全部账户] (data = -1)，仅查询区下拉框需要
    if (combo == comboQueryAccount) {
        combo->addItem("[全部账户]", -1);
    }
    for (size_t i = 0; i < accounts.size(); i++) {
        auto* acc = accounts[i];
        double bal = acc->getBalance();
        QString label = QString("[%1] %2 (余额: %3)")
            .arg((int)i)
            .arg(QString::fromStdString(acc->getId()))
            .arg(bal, 0, 'f', 2);
        combo->addItem(label, (int)i);
    }
}

void MainWindow::refreshAllCombos() {
    refreshAccountCombo(comboTxAccount);
    refreshAccountCombo(comboQueryAccount);
}

// ==================== 仪表盘刷新 ====================

void MainWindow::refreshDashboard() {
    // 1. 信用卡欠款提醒
    bool hasDebt = false;
    QString debtMsg;
    for (auto* acc : accounts) {
        auto* ca = dynamic_cast<CreditAccount*>(acc);
        if (ca && ca->getDebt() > 0) {
            hasDebt = true;
            debtMsg += QString("账户 %1 欠款: %2\n")
                .arg(QString::fromStdString(ca->getId()))
                .arg(ca->getDebt(), 0, 'f', 2);
        }
    }
    lblCreditWarning->setVisible(hasDebt);
    if (hasDebt) {
        lblCreditWarning->setText("⚠️ 信用卡欠款提醒：\n" + debtMsg + "请注意按期还款！");
    }

    // 2. 当月收支统计
    double income = 0.0, expense = 0.0;
    int targetYear = currentDate.getYear();
    int targetMonth = currentDate.getMonth();

    for (const auto& kv : Account::recordMap) {
        const AccountRecord& rec = kv.second;
        int ry = rec.getDate().getYear();
        int rm = rec.getDate().getMonth();
        if (ry == targetYear && rm == targetMonth) {
            // 只统计当前用户的账户
            bool isMyAccount = false;
            for (auto* acc : accounts) {
                if (acc == rec.getAccount()) { isMyAccount = true; break; }
            }
            if (isMyAccount) {
                double amt = rec.getAmount();
                if (amt > 0) income += amt;
                else expense += (-amt);
            }
        }
    }
    lblIncome->setText(QString::number(income, 'f', 2));
    lblExpense->setText(QString::number(expense, 'f', 2));
}

// ==================== 开户 ====================

void MainWindow::onCreateAccount() {
    std::string id = editOpenId->text().toStdString();
    if (id.empty()) { QMessageBox::warning(this, "错误", "账号不能为空"); return; }

    double rate = editOpenRate->text().toDouble();
    // 利率合法性校验：必须介于 0 和 1 之间
    if (rate > 1.0 || rate < 0.0) {
        QMessageBox::warning(this, "错误", "利率必须介于 0 和 1 之间！");
        return;
    }

    if (comboAccountType->currentData().toString() == "s") {
        accounts.push_back(new SavingsAccount(currentDate, id, rate));
        std::ostringstream oss;
        oss << "a s " << id << " " << rate;
        logCommand(oss.str());
    } else {
        double credit = editOpenCredit->text().toDouble();
        double fee    = editOpenFee->text().toDouble();
        accounts.push_back(new CreditAccount(currentDate, id, credit, rate, fee));
        std::ostringstream oss;
        oss << "a c " << id << " " << credit << " " << rate << " " << fee;
        logCommand(oss.str());
    }
    refreshAllCombos();
    refreshDashboard();
    QMessageBox::information(this, "成功", "账户创建成功！");
}

// ==================== 存款 ====================

void MainWindow::onDeposit() {
    if (comboTxAccount->currentIndex() < 0) return;
    int index = comboTxAccount->currentData().toInt();
    double amount = editTxAmount->text().toDouble();
    std::string desc = editTxDesc->text().toStdString();
    if (amount <= 0) { QMessageBox::warning(this, "错误", "金额必须大于0"); return; }

    try {
        accounts[index]->deposit(currentDate, amount, desc);
        std::ostringstream oss;
        oss << "d " << index << " " << roundAmount(amount) << " " << desc;
        logCommand(oss.str());
        refreshAllCombos();
        refreshDashboard();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "存款失败", e.what());
    }
}

// ==================== 取款 ====================

void MainWindow::onWithdraw() {
    if (comboTxAccount->currentIndex() < 0) return;
    int index = comboTxAccount->currentData().toInt();
    double amount = editTxAmount->text().toDouble();
    std::string desc = editTxDesc->text().toStdString();
    if (amount <= 0) { QMessageBox::warning(this, "错误", "金额必须大于0"); return; }

    try {
        accounts[index]->withdraw(currentDate, amount, desc);
        std::ostringstream oss;
        oss << "w " << index << " " << roundAmount(amount) << " " << desc;
        logCommand(oss.str());
        refreshAllCombos();
        refreshDashboard();
        QMessageBox::information(this, "成功", "取款成功！");
    } catch (const AccountException& ae) {
        QString msg = QString("账户 [%1] 操作失败: %2")
            .arg(QString::fromStdString(ae.getAccount()->getId()))
            .arg(ae.what());
        QMessageBox::warning(this, "交易失败", msg);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "交易失败", e.what());
    }
}

// ==================== 下一月 ====================

void MainWindow::onNextMonth() {
    if (currentDate.getMonth() == 12)
        currentDate = Date(currentDate.getYear() + 1, 1, 1);
    else
        currentDate = Date(currentDate.getYear(), currentDate.getMonth() + 1, 1);

    for (auto* acc : accounts) {
        try {
            acc->settle(currentDate);
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "结算错误", e.what());
        }
    }
    logCommand("n");
    refreshAllCombos();
    refreshDashboard();
    QMessageBox::information(this, "结算完成",
        QString("已进入 %1-%2-%3")
            .arg(currentDate.getYear())
            .arg(currentDate.getMonth())
            .arg(currentDate.getDay()));
}

// ==================== 跳转到本月某日 ====================

void MainWindow::onChangeDay() {
    int day = spinTargetDay->value();
    if (day < currentDate.getDay()) {
        QMessageBox::warning(this, "错误", "You cannot specify a previous day");
        return;
    }
    if (day > currentDate.getMaxDay()) {
        QMessageBox::warning(this, "错误", "Invalid day");
        return;
    }
    currentDate = Date(currentDate.getYear(), currentDate.getMonth(), day);
    std::ostringstream oss;
    oss << "c " << day;
    logCommand(oss.str());

    // 全局刷新
    lblCurrentDate->setText(QString("当前日期: %1-%2-%3")
        .arg(currentDate.getYear())
        .arg(currentDate.getMonth())
        .arg(currentDate.getDay()));
    refreshDashboard();
}

// ==================== 高级查询 ====================

void MainWindow::onQuery() {
    QDate from = dateEditFrom->date();
    QDate to   = dateEditTo->date();

    Date d1(from.year(), from.month(), from.day());
    Date d2(to.year(), to.month(), to.day());

    // 获取选中的账户数据
    int selIdx = comboQueryAccount->currentData().toInt();

    // 更新底部余额标签
    if (selIdx == -1) {
        // [全部账户]：累加所有账户余额
        double sum = 0.0;
        for (auto* acc : accounts) sum += acc->getBalance();
        lblSelectedBalance->setText(
            QString("当前所有账户余额总和: ￥%1")
                .arg(QString::number(sum, 'f', 2)));
    } else if (selIdx >= 0 && selIdx < (int)accounts.size()) {
        lblSelectedBalance->setText(
            QString("当前账户余额: ￥%1")
                .arg(QString::number(accounts[selIdx]->getBalance(), 'f', 2)));
    } else {
        lblSelectedBalance->setText("当前账户余额: ￥0.00");
    }

    // 从全局 recordMap 中提取区间内且属于当前用户的记录
    std::vector<const AccountRecord*> records;
    auto it1 = Account::recordMap.lower_bound(d1);
    auto it2 = Account::recordMap.upper_bound(d2);

    for (auto it = it1; it != it2; ++it) {
        const AccountRecord& rec = it->second;
        // 过滤：必须属于当前用户的账户
        bool isMyAccount = false;
        for (auto* acc : accounts) {
            if (acc == rec.getAccount()) { isMyAccount = true; break; }
        }
        if (!isMyAccount) continue;
        // 过滤：如果选中了具体账户（非 -1），则只显示该账户的账单
        if (selIdx >= 0 && rec.getAccount() != accounts[selIdx]) continue;
        records.push_back(&rec);
    }

    // 排序
    if (radioSortByAmount->isChecked()) {
        std::sort(records.begin(), records.end(),
            [](const AccountRecord* a, const AccountRecord* b) {
                return std::abs(a->getAmount()) > std::abs(b->getAmount());
            });
    }
    // 默认按时间（recordMap 已按 Date 有序）

    // 填充表格
    queryTable->setRowCount((int)records.size());
    for (int i = 0; i < (int)records.size(); i++) {
        const AccountRecord& r = *records[i];
        char dateBuf[32];
        snprintf(dateBuf, sizeof(dateBuf), "%d-%d-%d",
            r.getDate().getYear(), r.getDate().getMonth(), r.getDate().getDay());

        queryTable->setItem(i, 0, new QTableWidgetItem(dateBuf));
        queryTable->setItem(i, 1, new QTableWidgetItem(
            QString::fromStdString(r.getAccount()->getId())));
        queryTable->setItem(i, 2, new QTableWidgetItem(
            r.getAmount() > 0 ? "收入" : "支出"));
        queryTable->setItem(i, 3, new QTableWidgetItem(
            QString::number(r.getAmount(), 'f', 2)));
        queryTable->setItem(i, 4, new QTableWidgetItem(
            QString::number(r.getBalance(), 'f', 2)));
        queryTable->setItem(i, 5, new QTableWidgetItem(
            QString::fromStdString(r.getDesc())));
    }
}
