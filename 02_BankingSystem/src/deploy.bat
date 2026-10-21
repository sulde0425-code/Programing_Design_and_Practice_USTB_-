@echo off
chcp 65001 >nul
echo ============================================
echo   银行账户管理系统 - 一键打包脚本
echo ============================================
echo.

:: ===== 第一步：定位 windeployqt =====
echo [1/3] 正在查找 windeployqt.exe ...

set "WINDEPLOYQT="

:: 尝试从 PATH 中直接调用
where windeployqt >nul 2>&1
if %errorlevel% equ 0 (
    set "WINDEPLOYQT=windeployqt"
    goto :found
)

:: 尝试常见 Qt6 安装路径
for %%d in (
    "C:\Qt\6.9.1\mingw_64\bin\windeployqt.exe"
    "C:\Qt\6.8.0\mingw_64\bin\windeployqt.exe"
    "C:\Qt\6.7.0\mingw_64\bin\windeployqt.exe"
    "C:\Qt\6.6.0\mingw_64\bin\windeployqt.exe"
    "C:\Qt\6.5.0\mingw_64\bin\windeployqt.exe"
    "C:\Qt\5.15.2\mingw81_64\bin\windeployqt.exe"
    "C:\Qt\5.15.2\msvc2019_64\bin\windeployqt.exe"
) do (
    if exist %%d (
        set "WINDEPLOYQT=%%~d"
        goto :found
    )
)

echo [错误] 找不到 windeployqt.exe！
echo 请确保已安装 Qt，或将 Qt 的 bin 目录添加到系统 PATH。
echo 常见路径: C:\Qt\6.x.x\mingw_64\bin
pause
exit /b 1

:found
echo [OK]  找到: %WINDEPLOYQT%
echo.

:: ===== 第二步：拖入或指定 exe 文件 =====
echo [2/3] 请将 bank.exe 拖入此窗口，或输入完整路径：
echo.
if "%~1"=="" (
    set /p "EXE_PATH=  exe 路径: "
) else (
    set "EXE_PATH=%~1"
)

:: 去除引号
set "EXE_PATH=%EXE_PATH:"=%"

if not exist "%EXE_PATH%" (
    echo [错误] 文件不存在: %EXE_PATH%
    pause
    exit /b 1
)

echo [OK]  目标: %EXE_PATH%
echo.

:: ===== 第三步：创建打包目录并运行 windeployqt =====
echo [3/3] 正在打包...

:: 在 exe 所在目录创建发布文件夹
for %%F in ("%EXE_PATH%") do (
    set "EXE_DIR=%%~dpF"
    set "EXE_NAME=%%~nF"
)

set "DEPLOY_DIR=%EXE_DIR%%EXE_NAME%_Deploy"

:: 如果部署目录已存在，清理它
if exist "%DEPLOY_DIR%" (
    echo [清理] 删除旧的部署目录...
    rmdir /s /q "%DEPLOY_DIR%"
)

mkdir "%DEPLOY_DIR%"

:: 复制 exe 到部署目录
copy /y "%EXE_PATH%" "%DEPLOY_DIR%\" >nul

:: 如果有 commands*.txt 或 users.json，一并复制
for %%f in ("%EXE_DIR%commands*.txt" "%EXE_DIR%users.json") do (
    if exist %%f copy /y %%f "%DEPLOY_DIR%\" >nul 2>&1
)

:: 核心步骤：运行 windeployqt
echo.
echo   正在调用 windeployqt 自动提取依赖 DLL ...
echo   这可能需要 10-30 秒，请耐心等待...
echo.

"%WINDEPLOYQT%" "%DEPLOY_DIR%\%EXE_NAME%.exe" --no-translations

if %errorlevel% neq 0 (
    echo [错误] windeployqt 执行失败！
    pause
    exit /b 1
)

echo.
echo ============================================
echo   ✓ 打包完成！
echo ============================================
echo.
echo   独立运行文件夹:
echo     %DEPLOY_DIR%
echo.
echo   你可以将整个文件夹复制到任何 Windows 电脑
echo   （无需安装 Qt），直接双击 %EXE_NAME%.exe 即可运行！
echo.
echo   文件列表:
dir /b "%DEPLOY_DIR%"
echo.
pause
