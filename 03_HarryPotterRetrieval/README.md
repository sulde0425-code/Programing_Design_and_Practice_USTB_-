# 哈利波特文本检索系统

> **安装文档**: [INSTALL.md](INSTALL.md) - 包含 Linux/macOS/Windows（重点 Windows）环境安装与配置步骤
> **IDE 编译指南**: [VisualStudio编译指南.md](VisualStudio编译指南.md) - Visual Studio 2019/2022 下的 CMake 编译步骤与排错
> **实验报告**: [docs/HarryPotter_Report.pdf](docs/HarryPotter_Report.pdf)

快速开始（需要 Qt 6 与 CMake ≥ 3.16）：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64"
cmake --build build
```

只编译无 Qt 依赖的命令行版本：`cmake -S . -B build -DBUILD_GUI=OFF`。

## 题目要求

### 问题描述

将哈利波特的7本书（txt格式）读入，然后在指定了人名/地名后，显示查询结果，选择指定查询结果序号，能够显示指定查询结果所在位置前后的一段文字。

### 输入形式

- 哈利波特的7本书，txt文件

### 输出形式

**查询结果显示：**

```
序号    人名/地名    页码    章节    书名
1       Harry        1       1       Harry_Potter_and_the_Chamber_of_Secrets
2       Harry        2       1       Harry_Potter_and_the_Chamber_of_Secrets
...
```

**选择序号后显示上下文：**

选择序号1，显示该位置前后的一段文字（上一段落、当前段落、下一段落）：

```
Not for the first time, an argument had broken out over breakfast at
number four, Privet Drive. Mr. Vernon Dursley had been woken in the
early hours of the morning by a loud, hooting noise from his nephew
Harry's room.
```

### 界面要求

1. 支持输入人名/地名进行查询
2. 查询结果多条时，以列表形式显示并编号
3. 选择序号或点击记录行，显示该位置前后的文字
4. 建议使用 Qt 图形界面

---

## 项目结构

```
HarryPotter/
├── CMakeLists.txt              # CMake 构建配置（可选）
├── Makefile                    # 跨平台构建文件
├── README.md                   # 本文档
├── INSTALL.md                  # 环境安装与配置说明
│
├── data/                       # 数据目录（7本哈利波特电子书）
│   └── *.txt                   # 文本文件
│
├── static/                     # Web 静态资源
│   ├── index.html              # Web 检索界面
│   └── upload.html             # Web 上传界面
│
└── src/                        # 源代码目录
    ├── cli_main.cpp            # CLI 版本入口
    ├── web_main.cpp            # Web 版本入口
    ├── main.cpp                # GUI 版本入口
    │
    ├── core/                   # 核心算法模块（学生实现）
    │   ├── TextProcessor.h/cpp # 文本预处理
    │   ├── InvertedIndex.h/cpp # 倒排索引
    │   ├── SearchEngine.h/cpp  # 搜索引擎
    │   ├── Trie.h/cpp          # 前缀树（自动补全）
    │   ├── Similarity.h/cpp    # 相似度计算
    │   └── StringMatch.h/cpp   # 字符串匹配算法对比
    │
    ├── web/                    # Web 服务器模块（框架提供）
    │   └── HttpServer.h/cpp    # HTTP 服务器
    │
    └── gui/                    # GUI 模块（框架提供）
        └── MainWindow.h/cpp    # Qt 主窗口
```

> 说明：`build/`、`build-cmake/` 等目录为编译时自动生成，不计入项目静态结构。

---

## 构建文件职责：`Makefile` vs `CMakeLists.txt`

两者都用于“构建项目”，但职责层次不同：

- `Makefile`：
  - 是本项目直接可执行的构建脚本（偏“项目级快捷入口”）。
  - 已封装好目标：`make cli`、`make web`、`make gui`、`make all`。
  - 适合 Linux/macOS 和已安装 `make` 的环境，命令短、上手快。

- `CMakeLists.txt`：
  - 是跨平台构建描述文件（偏“生成器配置”）。
  - 先由 `cmake -S . -B build` 生成对应平台的构建系统，再 `cmake --build build` 调用编译器。
  - 更适合 Windows/Visual Studio 场景，也便于 IDE 集成。

本项目当前的功能覆盖差异：

- `Makefile`：支持 CLI / Web / GUI 三个目标。
- `CMakeLists.txt`：默认构建 CLI，`-DBUILD_GUI=ON` 可构建 GUI；Web 目标暂未纳入 CMake。

建议使用方式：

- 想快速跑通课程框架：优先 `make`。
- 在 Windows（尤其是 Visual Studio）上：优先 CMake 流程。

---

## 核心模块说明

### 1. 文本处理与索引构建

| 文件 | 函数 | 说明 |
|------|------|------|
| TextProcessor.cpp | `toLowerCase()` | 字符串转小写 |
| TextProcessor.cpp | `tokenize()` | 文本分词 |
| InvertedIndex.cpp | `addDocument()` | 构建倒排索引 |
| InvertedIndex.cpp | `getTopTerms()` | 获取高频词 |

**数据结构：**
```cpp
struct Posting {
    int docId;      // 书籍ID
    int chapter;    // 章节号
    int line;       // 行号
    int position;   // 单词位置
};

// 倒排索引: 单词 -> 出现位置列表
std::map<std::string, std::vector<Posting>> index_;
```

### 2. 搜索功能

| 文件 | 函数 | 说明 |
|------|------|------|
| SearchEngine.cpp | `search()` | 单词搜索 |
| SearchEngine.cpp | `searchAND()` | AND 查询 |
| SearchEngine.cpp | `searchOR()` | OR 查询 |
| SearchEngine.cpp | `searchPhrase()` | 短语搜索 |

### 3. 上下文显示

显示搜索结果所在位置的前后段落（上一段落、当前段落、下一段落）。

### 4. 字符串匹配算法对比

| 算法 | 时间复杂度 | 核心思想 |
|------|------------|----------|
| Brute Force | O(nm) | 逐位置尝试匹配 |
| KMP | O(n+m) | 失败函数避免重复比较 |
| Boyer-Moore | O(n/m)~O(nm) | 从右向左 + 坏字符规则 |
| Rabin-Karp | O(n+m) 平均 | 滚动哈希快速比较 |

## 编译与运行

### 编译

```bash
cd HarryPotter

# 编译 CLI 版本（无依赖）
make cli

# 编译 Web 版本（无依赖）
make web

# 编译 GUI 版本（需要 Qt）
make gui

# 编译全部（CLI + Web）
make all
```

> 说明：`build/` 目录由 `make` 自动生成，首次运行前请先完成至少一次编译（如 `make cli` / `make web` / `make all`）。
> 项目结构图中不展示 `build/`，因为它属于构建产物目录。

### 运行

```bash
# CLI 版本
./build/HarryPotterCLI [data_directory]
> load hp1.txt Harry Potter Book 1
> search Harry
> context 1

# Web 版本
./build/HarryPotterWeb [port]
# 打开浏览器访问 http://localhost:8080/upload.html 上传 txt 后再检索

# GUI 版本
./build/HarryPotterGUI
```

---

## 功能演示

### 1. 加载书籍

```
> load data/hp1.txt Harry Potter and the Philosopher's Stone
Loaded: [1] Harry Potter and the Philosopher's Stone - 6723 lines

> load data/hp2.txt Harry Potter and the Chamber of Secrets
Loaded: [2] Harry Potter and the Chamber of Secrets - 4347 lines
```

### 2. 搜索人名

```
> search Harry

Found 1247 occurrences of 'Harry':

序号    人名/地名       行号    章节    书名
----------------------------------------------------------------------
     1    Harry              5       1    Harry Potter and the Chamber of Secrets
     2    Harry              9       1    Harry Potter and the Chamber of Secrets
     3    Harry             16       1    Harry Potter and the Chamber of Secrets
...

Use 'context <n>' to view context for result #n
```

### 3. 查看上下文

```
> context 1

=== Context for result #1 ===
Book: Harry Potter and the Chamber of Secrets
Chapter: 1, Line: 5

--- Previous paragraph ---
(空)

--- Current paragraph ---
Harry tried, yet again, to explain.

--- Next paragraph ---
Harry tried to argue back but his words were drowned by a long,
loud belch from the Dursleys' son, Dudley.
```

---

## 学生实现清单

以下清单用于跟踪学生实现进度。

### 文本处理与索引构建（6分）

- [ ] **TextProcessor.cpp**
  - [ ] `toLowerCase()` - 字符串转小写
  - [ ] `tokenize()` - 文本分词

- [ ] **InvertedIndex.cpp**
  - [ ] `addDocument()` - 构建倒排索引
  - [ ] `getTopTerms()` - 获取高频词

### 搜索功能（6分）

- [ ] **SearchEngine.cpp**
  - [ ] `search()` - 单词搜索（已提供框架）
  - [ ] `searchAND()` - AND 查询
  - [ ] `searchOR()` - OR 查询
  - [ ] `searchPhrase()` - 短语搜索

### 上下文显示（4分）

- [ ] 显示当前段落（包含搜索词的段落）
- [ ] 显示前后段落（上一段落和下一段落）

### 字符串匹配算法对比（4分）

**算法实现（2分）：**
- [ ] **StringMatch.cpp**
  - [ ] `bruteForce()` - 暴力匹配
  - [ ] `kmp()` - KMP 算法
  - [ ] `boyerMoore()` - Boyer-Moore 算法
  - [ ] `rabinKarp()` - Rabin-Karp 算法

**可视化展示（2分）：**
- [ ] 对比表格输出（格式化显示匹配数、比较次数、时间）
- [ ] 性能分析图表（柱状图或折线图展示算法对比）

---

## 评分标准（满分20分）

| 模块 | 分值 | 说明 |
|------|------|------|
| 文本处理与索引构建 | 6分 | 分词、构建倒排索引 |
| 搜索功能 | 6分 | 单词搜索、布尔查询 |
| 上下文显示 | 4分 | 显示前后段落 |
| 字符串匹配算法对比 | 4分 | 实现算法(2分) + 可视化(2分) |
| **合计** | **20分** | |

---

## Web API 接口

| 接口 | 方法 | 参数 | 说明 |
|------|------|------|------|
| `/api/upload` | POST | Query: `name`, `title`(可选) + Body: txt内容 | 上传并加载文本文件 |
| `/api/load` | GET | `file`, `name` | 从服务端文件系统加载（兼容旧方式） |
| `/api/files` | GET | - | 列出已加载的书籍 |
| `/api/search` | GET | `q`, `mode` | 搜索（mode: single/and/or/phrase）|
| `/api/context` | GET | `doc`, `line` | 获取上下文（前后段落）|
| `/api/stats` | GET | - | 统计信息 |
| `/api/top` | GET | `n` | 高频词排行 |
| `/api/autocomplete` | GET | `prefix` | 自动补全 |
| `/api/spelling` | GET | `word` | 拼写建议 |
| `/api/compare` | GET | `pattern` | 算法性能对比 |

**示例：**
```bash
# 上传书籍（推荐）
curl -X POST "http://localhost:8080/api/upload?name=hp1.txt&title=Harry Potter Book 1" \
  --data-binary @data/hp1.txt

# 搜索
curl "http://localhost:8080/api/search?q=Harry&mode=single"

# 获取上下文
curl "http://localhost:8080/api/context?doc=1&line=100"

# 算法对比
curl "http://localhost:8080/api/compare?pattern=Harry"
```

---

## 跨平台支持

| 平台 | CLI | Web | GUI | 编译器 |
|------|:---:|:---:|:---:|--------|
| Linux | ✓ | ✓ | ✓* | g++, clang++ |
| macOS | ✓ | ✓ | ✓* | clang++ |
| Windows | ✓ | ✓ | ✓* | MinGW, MSVC |

> *GUI 版本需要安装 Qt5 或 Qt6

---

## 安装与环境配置

安装步骤已单独放在 [INSTALL.md](INSTALL.md)（包含 Linux/macOS/Windows，重点说明 Windows + MSYS2 + g++ 配置）。

---

## 参考资料

**数据结构：**
- 倒排索引: https://en.wikipedia.org/wiki/Inverted_index
- Trie 前缀树: https://en.wikipedia.org/wiki/Trie

**字符串匹配算法：**
- KMP 算法: https://en.wikipedia.org/wiki/Knuth–Morris–Pratt_algorithm
- Boyer-Moore 算法: https://en.wikipedia.org/wiki/Boyer–Moore_string-search_algorithm
- Rabin-Karp 算法: https://en.wikipedia.org/wiki/Rabin–Karp_algorithm

**相似度与纠错：**
- 编辑距离: https://en.wikipedia.org/wiki/Levenshtein_distance
- TF-IDF: https://en.wikipedia.org/wiki/Tf–idf
- BM25: https://en.wikipedia.org/wiki/Okapi_BM25

**开发工具：**
- Qt 文档: https://doc.qt.io/
