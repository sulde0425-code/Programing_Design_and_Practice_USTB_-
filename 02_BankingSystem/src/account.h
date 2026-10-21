#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "date.h"
#include <string>
#include <map>
#include <stdexcept>
#include <vector>
#include <algorithm>

// 前置声明
class Account;
class AccountRecord;

// 自定义账户异常类
class AccountException : public std::runtime_error {
private:
    const Account* account;
public:
    AccountException(const Account* acc, const std::string& msg)
        : std::runtime_error(msg), account(acc) {}
    const Account* getAccount() const { return account; }
};

// 账目记录类：封装单条交易流水
class AccountRecord {
private:
    Date date;
    const Account* account;
    double amount;
    double balance;
    std::string desc;

public:
    AccountRecord(const Date& date, const Account* account,
                  double amount, double balance, const std::string& desc);
    void show() const;

    // 用于 Qt 查询和排序的 getter
    const Date& getDate() const { return date; }
    const Account* getAccount() const { return account; }
    double getAmount() const { return amount; }
    double getBalance() const { return balance; }
    const std::string& getDesc() const { return desc; }
};

// 累加器类：按天累加余额，用于计算日均余额
class Accumulator {
private:
    Date lastDate;   // 上一次变更的日期
    double value;    // 当前数值（余额）
    double sum;      // 按日累加和

public:
    Accumulator(const Date& date, double value);
    
    // 计算截止到指定日期的累加总和
    double getSum(const Date& date) const;
    
    // 在指定日期更新数值，先将上一阶段的累加值计入 sum
    void change(const Date& date, double value);
    
    // 重置累加器
    void reset(const Date& date, double value);
};

// 账户基类
class Account {
protected:
    std::string id;       // 账号
    double balance;       // 余额
    static double total;  // 所有账户的总金额

    Account(const Date& date, const std::string& id);
    
    // 记录一笔交易
    void record(const Date& date, double amount, const std::string& desc);
    
    // 报错提示
    void error(const std::string& msg) const;

public:
    // 输出账户 ID 和余额
    virtual void show() const;
    
    std::string getId() const;
    double getBalance() const;

    virtual ~Account() {}

    // 纯虚函数 —— 强制派生类重写
    virtual void deposit(const Date& date, double amount, const std::string& desc) = 0;
    virtual void withdraw(const Date& date, double amount, const std::string& desc) = 0;
    virtual void settle(const Date& date) = 0;

    // 静态函数，返回全局总金额
    static double getTotal();

    // 全局交易记录（multimap，按日期排序）-- public 供外部查询
    static std::multimap<Date, AccountRecord> recordMap;

    // 按日期区间查询历史账目
    static void query(const Date& date1, const Date& date2);
};

// 储蓄账户类
class SavingsAccount : public Account {
private:
    Accumulator acc;  // 利息累加器（按日累加余额）
    double rate;      // 年利率

public:
    SavingsAccount(const Date& date, const std::string& id, double rate);
    
    // 存款
    void deposit(const Date& date, double amount, const std::string& desc) override;
    
    // 取款
    void withdraw(const Date& date, double amount, const std::string& desc) override;
    
    // 每年1月1日结算利息
    void settle(const Date& date) override;
};

// 信用卡账户类
class CreditAccount : public Account {
private:
    Accumulator acc;  // 利息累加器（按日累加欠款）
    double credit;    // 信用额度
    double rate;      // 日利率
    double fee;       // 年费

public:
    CreditAccount(const Date& date, const std::string& id,
                  double credit, double rate, double fee);

    // 返回当前欠款（余额为负时返回 -balance，否则返回 0）
    double getDebt() const;

    // 返回可用信用额度
    double getAvailableCredit() const;

    // 存款（还款）
    void deposit(const Date& date, double amount, const std::string& desc) override;

    // 取款（消费）
    void withdraw(const Date& date, double amount, const std::string& desc) override;

    // 输出账户信息（重写基类）
    void show() const override;

    // 每月1日结算利息，1月1日额外扣年费
    void settle(const Date& date) override;
};

#endif
