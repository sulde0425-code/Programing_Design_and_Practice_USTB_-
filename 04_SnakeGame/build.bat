@echo off
chcp 65001 >nul
set PATH=C:\msys64\mingw64\bin;%PATH%
if exist SnakeGame.exe del SnakeGame.exe

echo 正在编译贪吃蛇游戏...
g++ -std=c++17 -static src\main.cpp src\Snake.cpp src\Food.cpp src\Map.cpp src\GameManager.cpp src\MoveStrategy.cpp -o SnakeGame.exe

if %errorlevel% == 0 (
    echo 编译成功！
    echo 运行游戏...
    SnakeGame.exe
) else (
    echo 编译失败！
)
pause
