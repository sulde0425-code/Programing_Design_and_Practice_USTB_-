/**
 * @file main.cpp
 * @brief 哈利波特文本检索系统 - 程序入口
 *
 * 本项目是一个综合性的文本检索实验，旨在帮助学生掌握：
 * 1. 文本预处理技术
 * 2. 倒排索引数据结构
 * 3. 多种检索算法
 * 4. STL 容器的运用
 * 5. Qt GUI 编程基础
 */

#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("Harry Potter Text Retrieval System");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("Programming Practice Course");

    MainWindow window;
    window.show();

    return app.exec();
}
