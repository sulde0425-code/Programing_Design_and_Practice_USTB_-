# 安装与环境配置

本文件用于说明开发环境安装。  
`README.md` 专注于框架功能与使用方式。

## 平台总览

| 平台 | 推荐编译器 | 依赖管理方式 | 备注 |
|------|------------|--------------|------|
| Linux | g++ / clang++ | 系统包管理器（apt/dnf/pacman） | CLI/Web 可直接构建 |
| macOS | clang++ | Xcode Command Line Tools / Homebrew | CLI/Web 可直接构建 |
| Windows | MinGW-w64 (g++) / MSVC | MSYS2（推荐） | 本文重点说明 Windows |

## Linux 安装（简要）

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install -y build-essential
```

### Fedora

```bash
sudo dnf groupinstall -y "Development Tools"
```

### Arch Linux

```bash
sudo pacman -S --needed base-devel gcc
```

## macOS 安装（简要）

### 1. 安装命令行编译工具

```bash
xcode-select --install
```

### 2. （可选）安装 Qt（用于 GUI）

```bash
brew install qt
```

## Windows 安装（重点）

### 1. 安装 g++（通过 MSYS2）

推荐使用 [MSYS2](https://www.msys2.org/) 获取 MinGW-w64 工具链（含 g++ 和 make）。

1. 下载并安装 MSYS2：https://www.msys2.org/
2. 安装完成后，打开 **MSYS2 UCRT64** 终端，执行：

```bash
# 首次全量更新（执行后若提示重启终端，先关闭再打开）
pacman -Syu

# 重开终端后继续更新
pacman -Su

# 安装 MinGW-w64 工具链（含 g++、gcc、make）
pacman -S --needed mingw-w64-ucrt-x86_64-toolchain make
```

3. 将 MinGW 的 `bin` 目录加入系统 `Path`（默认：`C:\msys64\ucrt64\bin`）：
   - 打开“系统属性” → “高级” → “环境变量”
   - 在“系统变量”的 `Path` 中新增 `C:\msys64\ucrt64\bin`

4. 验证安装（在 CMD / PowerShell 中）：

```cmd
g++ --version
make --version
```

### 2. 安装 Qt（可选，仅 GUI 版本）

GUI 版本需要 Qt5 或 Qt6，可选两种方式。

#### 方式一：Qt 官方在线安装器（推荐）

1. 前往 https://www.qt.io/download-qt-installer 下载安装器
2. 运行安装器并登录 Qt 账户
3. 组件建议勾选：
   - **Qt 6.x** -> **MinGW 64-bit**（与 MSYS2 工具链匹配）
   - 或 **Qt 5.15.x** -> **MinGW 64-bit**

#### 方式二：通过 MSYS2 安装 Qt

在 **MSYS2 UCRT64** 终端中执行：

```bash
# 安装 Qt6
pacman -S --needed mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-qt6-tools

# 或安装 Qt5
pacman -S --needed mingw-w64-ucrt-x86_64-qt5-base mingw-w64-ucrt-x86_64-qt5-tools
```

## 安装后验证与构建

在项目根目录执行：

```bash
# CLI 版本
make cli

# Web 版本
make web

# GUI 版本（需要 Qt）
make gui
```

