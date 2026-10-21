# 程序设计实践 · 课程作业代码整理

北京科技大学《程序设计实践》课程作业归档仓库。

- 作者：苏力德（学号 U202442521，计 2405）
- 语言：C++ / C++17
- 涉及技术：EasyX 图形库、Qt 6 Widgets、CMake、设计模式

## 项目一览

| 目录 | 项目 | 技术栈 | 说明 |
| --- | --- | --- | --- |
| [`01_PlaneWar`](01_PlaneWar) | 飞机大战（EaxtraPlane） | C++ / EasyX / MSVC | 分层的飞机射击游戏，含 Boss 战、道具、关卡数据 |
| [`02_BankingSystem`](02_BankingSystem) | 银行账户管理系统 | C++17 / Qt 6 Widgets / CMake | 账户开户、存取款、转账、流水查询的桌面程序 |
| [`03_HarryPotterRetrieval`](03_HarryPotterRetrieval) | 哈利波特文本检索系统 | C++17 / Qt 6 / CMake | 倒排索引 + Trie + 相似度排序，支持 CLI / GUI / Web 三种入口 |
| [`04_SnakeGame`](04_SnakeGame) | 贪吃蛇 | C++17 / 控制台 | 策略模式 + 单例模式重构的贪吃蛇 |
| [`05_CourseLabs`](05_CourseLabs) | 课程 Lab0–Lab6 | C++ / CMake | 上机实验代码与题面 PDF |
| [`06_Exercises`](06_Exercises) | 课后练习 | C++ | 内存管理与指针、调试与改错、面向类设计三章练习 |

## 目录结构

```
.
├── 01_PlaneWar/                 # 飞机大战（Visual Studio 工程）
│   ├── EaxtraPlane.sln
│   ├── EaxtraPlane/             # 源码 + assets
│   └── docs/                    # 功能设计说明
├── 02_BankingSystem/
│   ├── src/                     # CMake 源码
│   └── docs/                    # 实验报告（pdf/docx）
├── 03_HarryPotterRetrieval/
│   ├── src/core|gui|web/        # 核心算法 / 图形界面 / 内置 HTTP 服务
│   ├── data/                    # 7 部小说文本
│   ├── static/                  # Web 前端页面
│   └── docs/                    # 实验报告
├── 04_SnakeGame/
│   ├── src/
│   └── docs/                    # 实验报告（含 UML 类图）
├── 05_CourseLabs/Lab0…Lab6/
└── 06_Exercises/
    ├── MemoryAndPointers/
    ├── DebuggingAndCorrection/
    └── ObjectOrientedDesign/
```

## 编译说明

各项目均在自己的 README 中给出了具体步骤，通用的环境要求：

| 项目 | 依赖 | 编译方式 |
| --- | --- | --- |
| 01_PlaneWar | Visual Studio 2022 + EasyX | 打开 `EaxtraPlane.sln`，Release x64 生成 |
| 02_BankingSystem | Qt 6（Widgets）+ CMake ≥ 3.16 | `cmake -S src -B build -DCMAKE_PREFIX_PATH=<Qt6> && cmake --build build` |
| 03_HarryPotterRetrieval | Qt 6 + CMake ≥ 3.16（CLI 目标无需 Qt） | `cmake -S . -B build && cmake --build build` |
| 04_SnakeGame | MinGW g++（C++17） | 双击 `build.bat`，或 `g++ -std=c++17 -static src/*.cpp -o SnakeGame.exe` |
| 05_CourseLabs | CMake + 任意 C++17 编译器 | 每个 Lab 目录单独 `cmake -S . -B build` |
| 06_Exercises | 任意 C++ 编译器 | 单文件编译，如 `g++ 1_2_2.cpp -o 1_2_2` |

源码统一使用 **UTF-8** 编码（Visual Studio 工程已开启 `/utf-8`）。

## 关于本仓库的整理说明

整理上传时做了以下处理：

1. **去重留存最完整版本**
   - 飞机大战：原目录存在 `easyplanewar`（单文件早期版）、`EaxtraPlane`（开发目录）、`苏力德-U202442521-planewar`（最终提交版）三份，源码完全一致，保留最终提交版。
   - 银行系统：`bank/bank2…bank5` 为逐次迭代版本，保留最终 Qt 图形界面版。
   - 哈利波特检索：三份拷贝（含 Mac 解压残留、构建产物），保留框架完整、含 CLI/GUI/Web 三入口的一份。
   - 贪吃蛇：保留重构后的 `贪吃蛇2`（策略模式 + 单例模式版本）。
2. **规范命名**：目录按 `编号_项目名` 排列，源码统一小写短横线/下划线风格，报告集中于各项目 `docs/`。
3. **未纳入版本库**：可执行文件（`.exe`）、Qt 运行库（`.dll`）、编译中间产物（`.obj/.o/.pdb/.tlog`）、IDE 配置（`.vs/.idea/cmake-build-*`）与压缩包（`.rar/.zip`）。这些均可在本地重新生成。
