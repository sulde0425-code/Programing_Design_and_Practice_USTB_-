#include "date.h"
#include <cstdio>
#include <cmath>
#include <iostream>

// 计算从公元1年1月1日到指定日期的总天数
static int computeTotalDays(int year, int month, int day) {
    // 从公元1年到 year-1 年的总天数
    int y = year - 1;
    int days = y * 365 + y / 4 - y / 100 + y / 400;
    
    // 当年各月的天数
    int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (int m = 0; m < month - 1; m++) {
        days += monthDays[m];
    }
    
    // 闰年且过了2月，加一天
    if (month > 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        days += 1;
    }
    
    days += (day - 1);  // 当月天数（1月1日为第0天）
    return days;
}

Date::Date() : year(2000), month(1), day(1) {
    totalDays = computeTotalDays(year, month, day);
}

Date::Date(int year, int month, int day)
    : year(year), month(month), day(day) {
    totalDays = computeTotalDays(year, month, day);
}

int Date::getMaxDay() const {
    int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear()) {
        return 29;
    }
    return monthDays[month - 1];
}

bool Date::isLeapYear() const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

void Date::show() const {
    printf("%d-%d-%d", year, month, day);
}

int Date::distance(const Date& date) const {
    return std::abs(totalDays - date.totalDays);
}

bool Date::operator<(const Date& date) const {
    return totalDays < date.totalDays;
}

Date Date::read() {
    int year, month, day;
    char c1, c2;
    std::cin >> year >> c1 >> month >> c2 >> day;
    return Date(year, month, day);
}
