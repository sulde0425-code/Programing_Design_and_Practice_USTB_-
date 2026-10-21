/**
 * @file CompareWidget.h
 * @brief 字符串匹配算法对比可视化组件
 *
 * 功能：
 * 1. 柱状图对比各算法的比较次数
 * 2. 折线图展示不同模式串长度下的性能变化
 * 3. 高亮显示最优算法
 */

#ifndef COMPAREWIDGET_H
#define COMPAREWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <vector>
#include <string>
#include "../core/StringMatch.h"

/**
 * @struct CompareData
 * @brief 一次对比的数据
 */
struct CompareData {
    std::vector<MatchResult> results;   // 各算法的匹配结果
    std::string pattern;                 // 使用的模式串
    std::string textSource;              // 文本来源描述
};

/**
 * @struct TrendPoint
 * @brief 性能趋势数据点（不同模式串长度）
 */
struct TrendPoint {
    int patternLength;                   // 模式串长度
    std::vector<MatchResult> results;    // 该长度下各算法结果
};

/**
 * @class CompareWidget
 * @brief 算法对比可视化组件
 *
 * 使用 QPainter 绘制，不依赖 QtCharts 模块。
 */
class CompareWidget : public QWidget {
    Q_OBJECT

public:
    explicit CompareWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置对比数据并刷新柱状图
     */
    void setCompareData(const CompareData& data);

    /**
     * @brief 设置性能趋势数据并刷新折线图
     */
    void setTrendData(const std::vector<TrendPoint>& data);

    /**
     * @brief 清空所有数据
     */
    void clearData();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    // ===== 布局常量 =====
    static constexpr int MARGIN_LEFT   = 80;
    static constexpr int MARGIN_RIGHT  = 30;
    static constexpr int MARGIN_TOP    = 40;
    static constexpr int MARGIN_BOTTOM = 60;
    static constexpr int BAR_GAP       = 15;

    // ===== 算法颜色 =====
    std::vector<QColor> algoColors_;
    std::vector<QString> algoNames_;

    // ===== 数据 =====
    CompareData compareData_;
    std::vector<TrendPoint> trendData_;
    bool hasCompareData_ = false;
    bool hasTrendData_ = false;

    // ===== 交互 =====
    int hoveredBar_ = -1;   // 当前悬停的柱状图索引

    // ===== 绘制方法 =====
    void drawBarChart(QPainter& painter, const QRect& chartRect);
    void drawLineChart(QPainter& painter, const QRect& chartRect);
    void drawBarTooltip(QPainter& painter, int barIndex, const QRect& barRect);
    int  barIndexAtPos(const QPoint& pos);

    // ===== 辅助方法 =====
    int  findBestByComparisons(const std::vector<MatchResult>& results);
    int  findBestByTime(const std::vector<MatchResult>& results);
    QString formatNumber(long long num);
};

#endif // COMPAREWIDGET_H
