/**
 * @file CompareWidget.cpp
 * @brief 字符串匹配算法对比可视化组件实现
 */

#include "CompareWidget.h"
#include <QToolTip>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

CompareWidget::CompareWidget(QWidget *parent)
    : QWidget(parent) {
    setMouseTracking(true);
    setMinimumSize(500, 350);

    // 算法名称和对应颜色
    algoNames_ = {
        QString::fromUtf8("Brute Force"),
        QString::fromUtf8("KMP"),
        QString::fromUtf8("Boyer-Moore"),
        QString::fromUtf8("Rabin-Karp")
    };

    // 使用柔和的配色方案
    algoColors_ = {
        QColor(66, 133, 244),    // 蓝色 - Brute Force
        QColor(234, 67, 53),     // 红色 - KMP
        QColor(52, 168, 83),     // 绿色 - Boyer-Moore
        QColor(251, 188, 4)      // 黄色 - Rabin-Karp
    };
}

void CompareWidget::setCompareData(const CompareData& data) {
    compareData_ = data;
    hasCompareData_ = true;
    update();
}

void CompareWidget::setTrendData(const std::vector<TrendPoint>& data) {
    trendData_ = data;
    hasTrendData_ = !data.empty();
    update();
}

void CompareWidget::clearData() {
    hasCompareData_ = false;
    hasTrendData_ = false;
    compareData_ = CompareData();
    trendData_.clear();
    update();
}

void CompareWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景
    painter.fillRect(rect(), QColor(255, 255, 255));

    int totalWidth = width();
    int totalHeight = height();

    if (!hasCompareData_ && !hasTrendData_) {
        // 没有数据时显示提示
        painter.setPen(QColor(150, 150, 150));
        QFont hintFont = painter.font();
        hintFont.setPointSize(12);
        painter.setFont(hintFont);
        painter.drawText(rect(), Qt::AlignCenter,
                         QString::fromUtf8("No comparison data yet.\n"
                                           "Upload files and run the comparison."));
        return;
    }

    // 根据数据情况决定布局
    bool showBar = hasCompareData_;
    bool showLine = hasTrendData_;

    if (showBar && showLine) {
        // 上下各一半
        int halfH = totalHeight / 2;
        QRect barRect(MARGIN_LEFT, 0, totalWidth - MARGIN_LEFT - MARGIN_RIGHT, halfH - 10);
        QRect lineRect(MARGIN_LEFT, halfH + 10, totalWidth - MARGIN_LEFT - MARGIN_RIGHT, halfH - 30);

        drawBarChart(painter, barRect);
        drawLineChart(painter, lineRect);
    } else if (showBar) {
        QRect barRect(MARGIN_LEFT, MARGIN_TOP,
                      totalWidth - MARGIN_LEFT - MARGIN_RIGHT,
                      totalHeight - MARGIN_TOP - MARGIN_BOTTOM);
        drawBarChart(painter, barRect);
    } else if (showLine) {
        QRect lineRect(MARGIN_LEFT, MARGIN_TOP,
                       totalWidth - MARGIN_LEFT - MARGIN_RIGHT,
                       totalHeight - MARGIN_TOP - MARGIN_BOTTOM);
        drawLineChart(painter, lineRect);
    }
}

// ==================== 柱状图 ====================

void CompareWidget::drawBarChart(QPainter& painter, const QRect& chartRect) {
    const auto& results = compareData_.results;
    if (results.empty()) return;

    int bestIdx = findBestByComparisons(results);
    int n = static_cast<int>(results.size());

    // 计算可用区域
    int chartW = chartRect.width();
    int chartH = chartRect.height();
    int barTotalWidth = (chartW - BAR_GAP * (n + 1)) / n;
    if (barTotalWidth < 20) barTotalWidth = 20;

    // 标题
    QFont titleFont = painter.font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(QColor(60, 60, 60));
    painter.drawText(chartRect.left(), chartRect.top() - 25, chartW, 25,
                     Qt::AlignLeft | Qt::AlignVCenter,
                     QString::fromUtf8("■ Comparison Count Comparison (lower is better)"));

    // 找出最大值用于缩放
    long long maxComp = 0;
    for (const auto& r : results) {
        if (r.comparisons > maxComp) maxComp = r.comparisons;
    }
    if (maxComp == 0) maxComp = 1;

    // Y轴
    painter.setPen(QColor(180, 180, 180));
    painter.drawLine(chartRect.left(), chartRect.top(),
                     chartRect.left(), chartRect.top() + chartH);

    // Y轴刻度线
    int numYTicks = 5;
    QFont tickFont = painter.font();
    tickFont.setPointSize(8);
    painter.setFont(tickFont);
    painter.setPen(QColor(130, 130, 130));

    for (int i = 0; i <= numYTicks; ++i) {
        int y = chartRect.top() + chartH - (chartH * i / numYTicks);
        painter.drawLine(chartRect.left() - 4, y, chartRect.left(), y);

        // 虚线参考线
        if (i > 0) {
            QPen dashPen(QColor(230, 230, 230));
            dashPen.setStyle(Qt::DashLine);
            painter.setPen(dashPen);
            painter.drawLine(chartRect.left(), y, chartRect.left() + chartW, y);
            painter.setPen(QColor(130, 130, 130));
        }

        long long val = maxComp * i / numYTicks;
        QString label = formatNumber(val);
        painter.drawText(chartRect.left() - 75, y - 8, 70, 16,
                         Qt::AlignRight | Qt::AlignVCenter, label);
    }

    // 绘制柱状图
    for (int i = 0; i < n; ++i) {
        const auto& r = results[i];
        int barX = chartRect.left() + BAR_GAP + i * (barTotalWidth + BAR_GAP);
        int barH = static_cast<int>(static_cast<double>(r.comparisons) / maxComp * chartH);
        if (barH < 2) barH = 2;
        int barY = chartRect.top() + chartH - barH;

        // 最优算法高亮
        QColor barColor = algoColors_[i % algoColors_.size()];
        if (i == bestIdx) {
            barColor = barColor.lighter(110);
        } else {
            barColor.setAlpha(180);
        }

        // 柱状图渐变
        QLinearGradient gradient(barX, barY, barX, barY + barH);
        gradient.setColorAt(0.0, barColor.lighter(130));
        gradient.setColorAt(0.3, barColor);
        gradient.setColorAt(1.0, barColor.darker(120));
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);

        // 圆角矩形
        painter.drawRoundedRect(barX, barY, barTotalWidth, barH, 4, 4);

        // 最优算法边框高亮
        if (i == bestIdx) {
            QPen highlightPen(QColor(220, 50, 50), 2.5);
            painter.setPen(highlightPen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(barX - 2, barY - 2, barTotalWidth + 4, barH + 4, 6, 6);
        }

        // 柱顶显示数值
        QFont valFont = painter.font();
        valFont.setPointSize(9);
        valFont.setBold(i == bestIdx);
        painter.setFont(valFont);
        painter.setPen(i == bestIdx ? QColor(200, 30, 30) : QColor(80, 80, 80));
        painter.drawText(barX - 5, barY - 22, barTotalWidth + 10, 20,
                         Qt::AlignCenter, formatNumber(r.comparisons));

        // X轴标签
        QFont labelFont = painter.font();
        labelFont.setPointSize(9);
        labelFont.setBold(i == bestIdx);
        painter.setFont(labelFont);
        painter.setPen(i == bestIdx ? QColor(200, 30, 30) : QColor(80, 80, 80));

        QString label = algoNames_[i % algoNames_.size()];
        if (i == bestIdx) {
            label += QString::fromUtf8(" ★");
        }
        painter.drawText(barX - 10, chartRect.top() + chartH + 5,
                         barTotalWidth + 20, 20,
                         Qt::AlignCenter, label);
    }

    // 图例：最优算法说明
    QFont legendFont = painter.font();
    legendFont.setPointSize(8);
    painter.setFont(legendFont);
    painter.setPen(QColor(100, 100, 100));
    painter.drawText(chartRect.left(), chartRect.top() + chartH + 40, chartW, 18,
                     Qt::AlignLeft,
                     QString::fromUtf8("★ = Best algorithm (fewest comparisons)"));
}

// ==================== 折线图 ====================

void CompareWidget::drawLineChart(QPainter& painter, const QRect& chartRect) {
    if (trendData_.empty()) return;

    int chartW = chartRect.width();
    int chartH = chartRect.height();

    // 标题
    QFont titleFont = painter.font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(QColor(60, 60, 60));
    painter.drawText(chartRect.left(), chartRect.top() - 25, chartW, 25,
                     Qt::AlignLeft | Qt::AlignVCenter,
                     QString::fromUtf8("● Performance vs Pattern Length (comparisons)"));

    // 找出所有数据点的最大值（用于Y轴缩放）
    long long maxComp = 0;
    int maxX = 0, minX = INT_MAX;
    for (const auto& tp : trendData_) {
        if (tp.patternLength > maxX) maxX = tp.patternLength;
        if (tp.patternLength < minX) minX = tp.patternLength;
        for (const auto& r : tp.results) {
            if (r.comparisons > maxComp) maxComp = r.comparisons;
        }
    }
    if (maxComp == 0) maxComp = 1;
    if (minX == maxX) maxX = minX + 10; // 避免除零

    // Y轴
    painter.setPen(QColor(180, 180, 180));
    painter.drawLine(chartRect.left(), chartRect.top(),
                     chartRect.left(), chartRect.top() + chartH);

    // X轴
    painter.drawLine(chartRect.left(), chartRect.top() + chartH,
                     chartRect.left() + chartW, chartRect.top() + chartH);

    // Y轴刻度
    int numYTicks = 5;
    QFont tickFont = painter.font();
    tickFont.setPointSize(8);
    painter.setFont(tickFont);
    painter.setPen(QColor(130, 130, 130));

    for (int i = 0; i <= numYTicks; ++i) {
        int y = chartRect.top() + chartH - (chartH * i / numYTicks);
        painter.drawLine(chartRect.left() - 4, y, chartRect.left(), y);

        if (i > 0) {
            QPen dashPen(QColor(230, 230, 230));
            dashPen.setStyle(Qt::DashLine);
            painter.setPen(dashPen);
            painter.drawLine(chartRect.left(), y, chartRect.left() + chartW, y);
            painter.setPen(QColor(130, 130, 130));
        }

        long long val = maxComp * i / numYTicks;
        painter.drawText(chartRect.left() - 75, y - 8, 70, 16,
                         Qt::AlignRight | Qt::AlignVCenter, formatNumber(val));
    }

    // 绘制每条算法的折线
    int numAlgos = trendData_[0].results.size();

    for (int algoIdx = 0; algoIdx < numAlgos; ++algoIdx) {
        QColor color = algoColors_[algoIdx % algoColors_.size()];
        color.setAlpha(220);

        QPen linePen(color, 2.5);
        linePen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(linePen);

        // 收集数据点
        std::vector<QPointF> points;
        for (const auto& tp : trendData_) {
            if (algoIdx < static_cast<int>(tp.results.size())) {
                double x = chartRect.left() +
                    static_cast<double>(tp.patternLength - minX) / (maxX - minX) * chartW;
                double y = chartRect.top() + chartH -
                    static_cast<double>(tp.results[algoIdx].comparisons) / maxComp * chartH;
                points.push_back(QPointF(x, y));
            }
        }

        // 绘制折线
        if (points.size() >= 2) {
            for (size_t i = 1; i < points.size(); ++i) {
                painter.drawLine(points[i - 1], points[i]);
            }
        }

        // 绘制数据点
        painter.setBrush(color.lighter(120));
        painter.setPen(QPen(color.darker(130), 1.5));
        for (const auto& pt : points) {
            painter.drawEllipse(pt, 4, 4);
        }

        // 在最右侧标注算法名
        if (!points.empty()) {
            QFont labelFont = painter.font();
            labelFont.setPointSize(8);
            labelFont.setBold(false);
            painter.setFont(labelFont);
            painter.setPen(color);
            QPointF lastPt = points.back();
            painter.drawText(static_cast<int>(lastPt.x()) + 6,
                             static_cast<int>(lastPt.y()) - 6,
                             algoNames_[algoIdx % algoNames_.size()]);
        }
    }

    // X轴标签
    QFont xLabelFont = painter.font();
    xLabelFont.setPointSize(9);
    painter.setFont(xLabelFont);
    painter.setPen(QColor(100, 100, 100));
    painter.drawText(chartRect.left(), chartRect.top() + chartH + 5,
                     chartW, 20, Qt::AlignCenter,
                     QString::fromUtf8("Pattern Length (characters)"));
}

// ==================== 工具提示 ====================

void CompareWidget::drawBarTooltip(QPainter& painter, int barIndex, const QRect& barRect) {
    Q_UNUSED(barRect);
    if (barIndex < 0 || barIndex >= static_cast<int>(compareData_.results.size())) return;
    Q_UNUSED(painter);
}

int CompareWidget::barIndexAtPos(const QPoint& pos) {
    Q_UNUSED(pos);
    return -1;
}

void CompareWidget::mouseMoveEvent(QMouseEvent* event) {
    Q_UNUSED(event);
}

void CompareWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}

// ==================== 辅助方法 ====================

int CompareWidget::findBestByComparisons(const std::vector<MatchResult>& results) {
    if (results.empty()) return -1;
    int best = 0;
    for (size_t i = 1; i < results.size(); ++i) {
        if (results[i].comparisons > 0 &&
            results[i].comparisons < results[best].comparisons) {
            best = static_cast<int>(i);
        }
    }
    return best;
}

int CompareWidget::findBestByTime(const std::vector<MatchResult>& results) {
    if (results.empty()) return -1;
    int best = 0;
    for (size_t i = 1; i < results.size(); ++i) {
        if (results[i].timeMs > 0 &&
            results[i].timeMs < results[best].timeMs) {
            best = static_cast<int>(i);
        }
    }
    return best;
}

QString CompareWidget::formatNumber(long long num) {
    if (num >= 1000000) {
        return QString::number(num / 1000000.0, 'f', 1) + "M";
    } else if (num >= 1000) {
        return QString::number(num / 1000.0, 'f', 1) + "K";
    }
    return QString::number(num);
}
