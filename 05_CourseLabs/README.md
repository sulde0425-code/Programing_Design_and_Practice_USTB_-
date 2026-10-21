# 课程上机实验 Lab0 – Lab6

《程序设计实践》上机实验代码，每题一个独立目录，包含源文件、CMake 工程与题面 PDF。

| 目录 | 主题 | 代码要点 |
| --- | --- | --- |
| `Lab0` | 奇偶数的位运算改写 | 用 `n >> 1` 代替除法，熟悉基础 IO 与模板化输入 |
| `Lab1` | 素数判断 | 试除法判素（`sqrt(x)` 上界），`vector` 与数学库的使用 |
| `Lab2` | AES S-Box / 字节代换 | 常量表 + 位运算，矩阵与查表实现 |
| `Lab3` | 词频统计 | C++17 `filesystem` 遍历目录、`map` 计数、字符串规范化与排序输出 |
| `Lab5` | 浏览历史管理（HistoryManager） | 自定义容器管理 URL 访问记录，支持 `VISIT` / `CLEAR` 等命令序列处理 |
| `Lab6` | 2048 游戏 | `Game2048` 类的棋盘合并逻辑、随机数生成与界面绘制 |

> 原作业中未见 Lab4 目录，故此处只有 Lab0–Lab3、Lab5、Lab6。

## 编译

每个实验目录都是独立 CMake 工程：

```bash
cd Lab1
cmake -S . -B build
cmake --build build
```

`Lab3` 运行前需保证 `test.txt` 与可执行文件在同一工作目录（仓库内已附带该测试文件）。
