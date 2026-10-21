# 课后练习

按章节分为三部分，均为单文件（或单工程）小练习，直接编译运行。

## MemoryAndPointers · 内存管理与指针

| 文件 | 内容 |
| --- | --- |
| `1_2_2.cpp` | 星球/行星结构体与排序，求合并或移动的最小代价（`struct planet`） |
| `1_2_planets.cpp` | 以中位数为基准计算移动总代价 |
| `1_3.cpp` | 按行读入并切分数据（`getline` + `stringstream`）的输入处理练习 |
| `memory.cpp` | `vector` 模拟内存块查找，练习容器与引用传参 |
| `stepback.cpp` | 指针与数组操作：把数组循环后移 `m` 位 |

## DebuggingAndCorrection · 调试与改错

| 文件 | 内容 |
| --- | --- |
| `3_1_2.cpp` | 大数卷积（读入数字字符串 → 逐位相乘 → 进位处理） |
| `3_3.cpp` | 借助异或运算交换两个变量，含调试改错过程 |
| `error_stack/` | 表达式求值工程（Visual Studio `.vcxproj`）：`Token` / `Visitor` / `ExprToken` 词法分析、运算符优先级归约与错误处理 |

`error_stack` 为多文件工程，用 Visual Studio 打开 `error_stack.vcxproj` 编译；其余为单文件：

```bash
g++ -std=c++17 stepback.cpp -o stepback
```

## ObjectOrientedDesign · 面向类设计

| 文件 | 内容 |
| --- | --- |
| `2_1_curriculum.cpp` | `Curriclum` 课程类：封装属性与成员函数 |
| `2_2_student.cpp` | `Student` 类与对象排序 |
| `2_3_game.cpp` | `Point2D` 等游戏对象类的构造与运算 |
| `2_4_my_stack.cpp` | `MyStack` 栈类的封装（数组实现） |
| `devices.cpp` | `PoweredDevice`：静态成员计数与继承 |
| `3_1_big_number.cpp` / `3_1_1_big_number_v2.cpp` | 大数卷积类实现的两个版本（后者为改进版） |
| `mooc_class_template.cpp` | 类模板 `Student<T, size>`，非类型模板参数与 `initializer_list` |
