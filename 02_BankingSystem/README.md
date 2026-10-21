# 个人银行账户管理系统（Qt 版）

在命令行版银行账户系统的基础上，用 **Qt 6 Widgets** 重构为图形界面程序，并补充了异常处理、仪表盘与账单排序等功能。

## 功能

- 用户注册 / 登录（`AuthDialog`），多账户隔离
- 储蓄账户、信用账户两类账户模型，支持开户、存款、取款、转账
- 基于 `std::runtime_error` 的 `AccountException` 异常体系：超额取现、日期非法等情况由底层抛出，UI 层 `try-catch` 拦截并弹窗提示；仅当业务成功时才写入命令日志，避免错误操作污染历史记录
- 仪表盘：实时显示银行总资产、信用卡还款提醒
- 流水查询：按月筛选，支持按时间顺序或按金额降序排序
- 数据持久化：账户数据与命令流写入本地文件，重启后可恢复

## 编译运行

需要 Qt 6（Widgets 模块）与 CMake ≥ 3.16。

```bash
cd src
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64"
cmake --build build
```

生成的 `bank.exe` 需要 Qt 运行库，可用同目录的 `deploy.bat`（调用 `windeployqt`）打包后再运行。
若使用 Qt 5，把 `CMakeLists.txt` 中的 `find_package(Qt6 ...)` 改为 `Qt5`，并同步修改链接目标。

## 代码结构

| 文件 | 职责 |
| --- | --- |
| `date.h/.cpp` | 日期类：闰年、月末天数、日期间隔计算，构造时校验合法性 |
| `account.h/.cpp` | `Account` 抽象基类 + `SavingsAccount` / `CreditAccount`，含 `Accumulator`（利息累加器）、`AccountRecord`（流水记录）、`AccountException` |
| `mainwindow.h/.cpp` | 主窗口：仪表盘、操作面板、流水表格与排序、槽函数中的异常拦截 |
| `main.cpp` | `QApplication` 入口 |
| `CMakeLists.txt` / `deploy.bat` | 构建与部署脚本 |

## 设计要点

- **异常安全的日志写入**：`onWithdraw()` 等槽函数先用 `try` 包裹核心业务，业务成功后才追加 `commands.txt`，保证日志与账户状态一致。
- **继承与多态**：`Account` 定义统一接口（`deposit` / `withdraw` / `settle` 等），储蓄与信用账户分别实现利息与还款规则。
- **UI 与业务分离**：界面层只负责取值、调用领域对象、展示结果，金额与日期的格式化集中在工具函数中。

实验报告见 [`docs/BankSystem_Report.pdf`](docs/BankSystem_Report.pdf)。
