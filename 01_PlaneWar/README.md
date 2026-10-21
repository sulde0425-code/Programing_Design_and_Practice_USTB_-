# 飞机大战 EaxtraPlane

基于 **C++ / EasyX** 的纵向卷轴射击游戏，采用面向对象分层设计，在基础飞机大战之上扩展了血量与无敌帧、Boss 战、道具系统与关卡数据配置。

## 运行环境

- Visual Studio 2022（v143 工具集，x64）
- [EasyX 图形库](https://easyx.cn/)（安装后 VS 会自动识别 `graphics.h`）
- 打开 `EaxtraPlane.sln`，选择 `Release | x64` 生成并运行

> 游戏窗口依赖 `assets/` 目录下的贴图，运行时工作目录需为可执行文件所在目录（资源加载器会自动向上查找 `assets/`、`../assets/`、`../../assets/`）。

## 操作方式

| 按键 | 功能 |
| --- | --- |
| `W A S D` / 方向键 | 移动战机（支持八向，对角线速度已归一化） |
| `空格` / `J` | 射击 |
| `B` | 使用炸弹（清屏） |
| `Enter` | 开始游戏 / 结算后确认 |
| `Esc` | 退出 |

## 代码结构

| 文件 | 职责 |
| --- | --- |
| `main.cpp` | 程序入口，初始化窗口与游戏循环 |
| `GameManager.h/.cpp` | 主循环、输入处理、碰撞检测、关卡与结算流程 |
| `GameObject.h` / `Aircraft.h` | 游戏对象与飞行器基类（继承体系根节点） |
| `Player.h/.cpp` | 玩家战机：血量、无敌帧、闪烁受击反馈、武器等级 |
| `Enemy.h/.cpp` | 敌机：多种运动模式与血量配置 |
| `Boss.h/.cpp` | Boss：多阶段弹幕与血量条 |
| `Bullet.h/.cpp` | 子弹（玩家 / 敌方通用） |
| `Prop.h/.cpp` | 道具：治疗、护盾、散射、炸弹 |
| `ResourceManager.h/.cpp` | 贴图与资源的统一加载与缓存 |
| `LevelData.h` | 关卡波次、敌人数量与属性配置 |
| `EasyXUtils.h/.cpp` | EasyX 绘制辅助（透明贴图、文字等） |

## 设计要点

- **受击反馈与生存机制**：`Player` 内部维护 `invincibleTimer` 帧计数器，受击后进入无敌状态，渲染层按奇偶帧交替绘制贴图实现街机式闪烁；物理层在无敌期间跳过扣血，物理流与渲染流解耦。
- **配置与逻辑分离**：关卡数据集中在 `LevelData.h`，调整波次与难度无需改动游戏逻辑。
- **资源集中管理**：`ResourceManager` 统一持有 `IMAGE` 对象，避免重复加载与析构混乱。

详细功能说明见 [`docs/PlaneWar_Design.docx`](docs/PlaneWar_Design.docx)。
