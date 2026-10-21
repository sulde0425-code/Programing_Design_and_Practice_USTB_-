#ifndef DATE_H
#define DATE_H

class Date {
private:
    int year;
    int month;
    int day;
    int totalDays;  // 自公元1年1月1日以来的总天数，用于计算日期差

public:
    Date();  // 默认构造函数
    Date(int year, int month, int day);
    
    int getYear() const { return year; }
    int getMonth() const { return month; }
    int getDay() const { return day; }
    
    int getMaxDay() const;       // 返回当前月最大天数
    bool isLeapYear() const;     // 判断是否为闰年
    void show() const;           // 格式化输出日期，如 2008-11-01
    int distance(const Date& date) const;  // 返回与目标日期的绝对天数差
    
    bool operator<(const Date& date) const;  // 小于运算符，用于 multimap 键排序
    static Date read();                       // 从 cin 读取 YYYY-MM-DD 格式日期
};

#endif
