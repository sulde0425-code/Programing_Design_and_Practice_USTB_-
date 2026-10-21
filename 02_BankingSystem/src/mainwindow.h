#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QRadioButton>
#include <QDateEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <fstream>

#include "account.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const std::string& username, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onCreateAccount();       // 开户
    void onDeposit();             // 存款
    void onWithdraw();            // 取款
    void onNextMonth();           // 进入下月
    void onChangeDay();           // 跳转到本月某日
    void onQuery();               // 高级查询
    void refreshDashboard();      // 刷新仪表盘

private:
    void logCommand(const std::string& cmd);
    void refreshAccountCombo(QComboBox* combo);
    void refreshAllCombos();      // 刷新所有账户下拉框
    std::string getLogFilename() const;

    // 核心数据
    std::string username;
    Date currentDate;
    std::vector<Account*> accounts;

    // UI 组件
    QTabWidget* tabWidget;

    // === 仪表盘 ===
    QLabel* lblCreditWarning;
    QLabel* lblIncome;
    QLabel* lblExpense;
    QLabel* lblCurrentDate;      // 当前日期显示

    // === 操作区 ===
    QComboBox* comboAccountType;
    QLineEdit* editOpenId;
    QLineEdit* editOpenRate;
    QLineEdit* editOpenCredit;
    QLineEdit* editOpenFee;

    QComboBox* comboTxAccount;
    QLineEdit* editTxAmount;
    QLineEdit* editTxDesc;

    QSpinBox* spinTargetDay;     // 跳转日期选择器
    QPushButton* btnChangeDay;   // 跳转按钮

    // === 查询区 ===
    QDateEdit* dateEditFrom;
    QDateEdit* dateEditTo;
    QComboBox* comboQueryAccount;  // 单账户筛选
    QRadioButton* radioSortByTime;
    QRadioButton* radioSortByAmount;
    QTableWidget* queryTable;
    QLabel* lblSelectedBalance;    // 选中账户余额显示
};

#endif // MAINWINDOW_H
