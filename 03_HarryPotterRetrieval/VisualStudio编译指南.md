# Visual Studio 编译指南（Windows）

本文档说明如何在 Windows 下使用 Visual Studio 编译 `HarryPotter_Framework`。

## 0. 版本说明

- 如果你说的是“Visual Studio 2020”，通常是口语说法；官方版本号里没有“2020”。
- 常见对应关系：
  - Visual Studio 2019（发布时间：2019 年）
  - Visual Studio 2022（发布时间：2021-11-08）

本文步骤适用于 VS2019 和 VS2022。

## 1. 环境准备

请先确保以下组件已安装：

1. Visual Studio（2019 或 2022）
2. 工作负载：`使用 C++ 的桌面开发`
3. CMake（通常随 VS 一起安装）

可在 `x64 Native Tools Command Prompt` 或 PowerShell 验证：

```powershell
cmake --version
cl
```

## 2. 用 Visual Studio IDE 直接编译（推荐）

### 2.1 打开项目

1. 打开 Visual Studio
2. `File -> Open -> Folder`
3. 选择目录：`HarryPotter_Framework`

Visual Studio 会自动识别 `CMakeLists.txt` 并开始配置。

### 2.2 选择配置并构建

1. 顶部选择配置：`x64-Debug` 或 `x64-Release`
2. 菜单 `Build -> Build All`
3. 在启动目标里选择 `HarryPotterCLI`
4. 运行（`Ctrl+F5`）

## 3. 命令行 CMake 构建（适合脚本/CI）

先进入项目目录：

```powershell
cd HarryPotter_Framework
```

### 3.1 VS2022（生成器：Visual Studio 17 2022）

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target HarryPotterCLI
.\build\Release\HarryPotterCLI.exe data
```

### 3.2 VS2019（生成器：Visual Studio 16 2019）

```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Release --target HarryPotterCLI
.\build\Release\HarryPotterCLI.exe data
```

## 4. GUI 版本（可选，需 Qt）

`CMakeLists.txt` 默认只构建 CLI。构建 GUI 需要打开 `BUILD_GUI` 并提供 Qt 路径。

示例（VS2022 + Qt6）：

```powershell
cmake -S . -B build-gui -G "Visual Studio 17 2022" -A x64 -DBUILD_GUI=ON -DCMAKE_PREFIX_PATH="C:\Qt\6.6.3\msvc2022_64"
cmake --build build-gui --config Release
```

注意事项：

1. Qt 编译器套件要和 VS 工具链匹配
2. VS2019 常用 `msvc2019_64`
3. VS2022 常用 `msvc2022_64`

## 5. 当前 CMake 目标说明

当前 `CMakeLists.txt` 的目标是：

1. `HarryPotterCLI`（默认可构建）
2. GUI 主程序（仅在 `-DBUILD_GUI=ON` 时构建）

说明：Web 目标当前未纳入 CMake 流程。

## 6. 常见问题排查

### 6.1 `cl` 不是内部或外部命令

原因：没有在 VS 开发者终端下执行，或未安装 C++ 工具链。  
处理：打开 `x64 Native Tools Command Prompt for VS`，确认安装了 `使用 C++ 的桌面开发`。

### 6.2 CMake 找不到 Qt（`Could not find Qt5/Qt6`）

处理：

1. 安装 Qt（Qt Maintenance Tool）
2. 确认 `CMAKE_PREFIX_PATH` 指向正确目录（如 `C:\Qt\6.6.3\msvc2022_64`）
3. 确认 Qt 套件与 VS 版本匹配

### 6.3 运行时提示找不到数据文件

处理：从项目根目录运行，或显式传入数据目录：

```powershell
.\build\Release\HarryPotterCLI.exe data
```
