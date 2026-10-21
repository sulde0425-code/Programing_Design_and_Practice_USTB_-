# 贪吃蛇 SnakeGame

控制台版贪吃蛇，用 **策略模式** 封装四种移动方向、**单例模式** 管理全局游戏状态，是设计模式章节的重构练习。

## 编译运行

Windows 下双击 `build.bat`（内部使用 MinGW g++ 并自动运行），或手动执行：

```bash
g++ -std=c++17 -static src/main.cpp src/Snake.cpp src/Food.cpp src/Map.cpp src/GameManager.cpp src/MoveStrategy.cpp -o SnakeGame.exe
```

Linux / macOS 去掉 `-static` 即可。

## 操作

| 按键 | 功能 |
| --- | --- |
| `W` `A` `S` `D` | 上 / 左 / 下 / 右 |
| `Esc` | 退出游戏 |

吃到食物得 10 分，撞墙或撞到自身游戏结束。

## 代码结构

| 文件 | 职责 |
| --- | --- |
| `GameManager.h/.cpp` | **单例**：持有地图、蛇、食物，驱动主循环、按键处理、碰撞判断与绘制 |
| `Snake.h/.cpp` | 蛇身坐标容器（`vector<pair<int,int>>`），持有当前 `MoveStrategy*`，负责移动与增长 |
| `MoveStrategy.h/.cpp` | **策略模式**：`UpStrategy` / `DownStrategy` / `LeftStrategy` / `RightStrategy` 四个具体策略 |
| `Food.h/.cpp` | 食物随机生成，避开蛇身 |
| `Map.h/.cpp` | 地图边框绘制与尺寸常量 |
| `main.cpp` | 程序入口 |

## 设计要点

- **策略模式**：方向切换通过替换策略对象完成，`Snake::Move()` 只依赖抽象策略接口，新增移动方式（如穿墙）无需修改蛇本身。
- **单例模式**：`GameManager::getInstance()` 保证地图、分数、游戏状态全局唯一，避免全局变量散落。
- **随机生成重试**：食物生成时若与蛇身重叠则重新抽取（`goto again`），保证合法性。

实验报告（含 UML 类图）见 [`docs/SnakeGame_Report.md`](docs/SnakeGame_Report.md)。
