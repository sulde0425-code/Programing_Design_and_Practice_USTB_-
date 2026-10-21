#include "account.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// 静态成员变量初始化
double Account::total = 0.0;
std::multimap<Date, AccountRecord> Account::recordMap;

// 金额四舍五入保留两位小数
static double roundAmount(double amount) {
    return std::floor(amount * 100 + 0.5) / 100.0;
}

// ==================== Accumulator 实现 ====================

Accumulator::Accumulator(const Date& date, double value)
    : lastDate(date), value(value), sum(0.0) {
}

double Accumulator::getSum(const Date& date) const {
    // sum + 从上次变更到指定日期的累加值
    return sum + date.distance(lastDate) * value;
}

void Accumulator::change(const Date& date, double value) {
    // 将上一阶段的 (天数差 × 旧值) 累加到 sum
    sum += date.distance(lastDate) * this->value;
    lastDate = date;
    this->value = value;
}

void Accumulator::reset(const Date& date, double value) {
    lastDate = date;
    this->value = value;
    sum = 0.0;
}

// ==================== AccountRecord 实现 ====================

AccountRecord::AccountRecord(const Date& date, const Account* account,
                             double amount, double balance, const std::string& desc)
    : date(date), account(account), amount(amount), balance(balance), desc(desc) {
}

void AccountRecord::show() const {
    date.show();
    std::cout << "\t#" << account->getId() << "\t"
              << amount << "\t" << balance << "\t"
              << desc << std::endl;
}

// ==================== Account 实现 ====================

Account::Account(const Date& date, const std::string& id)
    : id(id), balance(0.0) {
}

void Account::record(const Date& date, double amount, const std::string& desc) {
    AccountRecord rec(date, this, amount, balance, desc);
    recordMap.insert(std::make_pair(date, rec));
    rec.show();
}

void Account::error(const std::string& msg) const {
    std::cout << "Error(" << id << "): " << msg << std::endl;
}

void Account::show() const {
    std::cout << id << "\tBalance: "
              << balance << std::endl;
}

std::string Account::getId()const {
    return id;
}

double Account::getBalance()const {
    return balance;
}
double Account::getTotal() {
    return total;
}

void Account::query(const Date& date1, const Date& date2) {
    auto it1 = recordMap.lower_bound(date1);
    auto it2 = recordMap.upper_bound(date2);
    for (auto it = it1; it != it2; ++it) {
        it->second.show();
    }
}

// ==================== SavingsAccount 实现 ====================

SavingsAccount::SavingsAccount(const Date& date, const std::string& id, double rate)
    : Account(date, id), acc(date, 0.0), rate(rate) {
    date.show();
    std::cout << "\t#" << id << " " << "created" << std::endl;
}

void SavingsAccount::deposit(const Date& date, double amount, const std::string& desc) {
    amount = roundAmount(amount);
    double newBalance = balance + amount;
    // 先将旧余额按天数累加，再更新为新余额
    acc.change(date, newBalance);
    balance = newBalance;
    total += amount;
    record(date, amount, desc);
}

void SavingsAccount::withdraw(const Date& date, double amount, const std::string& desc) {
    amount = roundAmount(amount);
    if (balance < amount) {
        throw AccountException(this, "Insufficient funds");
    }
    double newBalance = balance - amount;
    acc.change(date, newBalance);
    balance = newBalance;
    total -= amount;
    record(date, -amount, desc);
}

void SavingsAccount::settle(const Date& date) {
    // 储蓄账户按年计息：仅在1月执行结算
    if (date.getMonth() != 1) return;
    
    // 获取截止到结算日的累加总和（即上一年的余额 × 天数累加）
    double accumulatedSum = acc.getSum(date);
    
    // 计算上一年的总天数
    int prevYear = date.getYear() - 1;
    bool isLeap = (prevYear % 4 == 0 && prevYear % 100 != 0) || (prevYear % 400 == 0);
    int daysInYear = isLeap ? 366 : 365;
    
    // 年利息 = 日均余额 × 年利率 = (累加和 / 年天数) × 年利率
    double interest = accumulatedSum * rate / daysInYear;
    interest = roundAmount(interest);
    
    if (interest > 0) {
        deposit(date, interest, "interest");
    }
    
    // 重置累加器，开始新一年的利息计算
    acc.reset(date, balance);
}

// ==================== CreditAccount 实现 ====================

CreditAccount::CreditAccount(const Date& date, const std::string& id,
                             double credit, double rate, double fee)
    : Account(date, id), acc(date, 0.0), credit(credit), rate(rate), fee(fee) {
    date.show();
    std::cout << "\t#" << id << " " << "created" << std::endl;
}

double CreditAccount::getDebt() const {
    return (balance < 0) ? -balance : 0.0;
}

double CreditAccount::getAvailableCredit() const {
    return credit + balance;
}

void CreditAccount::show() const {
    Account::show();
    std::cout << "Available credit: " << getAvailableCredit() << std::endl;
}

void CreditAccount::deposit(const Date& date, double amount, const std::string& desc) {
    amount = roundAmount(amount);
    double newBalance = balance + amount;
    double newDebt = (newBalance < 0) ? -newBalance : 0.0;
    // 累加旧欠款，更新为新欠款
    acc.change(date, newDebt);
    balance = newBalance;
    total += amount;
    record(date, amount, desc);
}

void CreditAccount::withdraw(const Date& date, double amount, const std::string& desc) {
    amount = roundAmount(amount);
    // 校验可用额度是否足够
    if (balance - amount < -credit) {
        throw AccountException(this, "Insufficient credit");
    }
    double newBalance = balance - amount;
    double newDebt = (newBalance < 0) ? -newBalance : 0.0;
    // 累加旧欠款，更新为新欠款
    acc.change(date, newDebt);
    balance = newBalance;
    total -= amount;
    record(date, -amount, desc);
}

void CreditAccount::settle(const Date& date) {
    // 1. 【关键修复】先强制将本阶段的欠款累加进 sum 中，同步累加器状态
    acc.change(date, getDebt());

    // 2. 获取当月累计的按日欠款和（此时 getSum 内部 distance 为 0，直接返回 sum）
    double debtSum = acc.getSum(date);
    double interest = roundAmount(debtSum * rate);

    // 3. 结算利息（直接扣除，不经过 withdraw 防止超额度被拦截）
    if (interest > 0) {
        balance -= interest;
        total -= interest;
        record(date, -interest, "interest");
    }

    // 4. 结算年费（仅每年 1 月 1 日）
    if (date.getMonth() == 1) {
        balance -= fee;
        total -= fee;
        record(date, -fee, "annual fee");
    }

    // 5. 重置累加器，开始下个月的周期
    // 传入扣除利息/年费后的最新欠款 getDebt()，确保下个月以此为基数计息
    acc.reset(date, getDebt());
}