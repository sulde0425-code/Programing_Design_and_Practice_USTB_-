#pragma once
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Boss.h"
#include "Prop.h"
#include "LevelData.h"
#include "EasyXUtils.h"
#include "ResourceManager.h"
#include <vector>

// ============================================================
// 游戏状态枚举
// ============================================================
enum GameState {
    STATE_MENU,        // 主菜单
    STATE_PLAYING,     // 游戏中
    STATE_SETTLEMENT,  // 结算选 Buff（Phase 3）
    STATE_GAMEOVER     // 游戏结束
};

// ============================================================
// GameManager - 游戏全局管理器
// ============================================================
class GameManager {
private:
    int       windowWidth;
    int       windowHeight;
    GameState gameState;
    Player*   player;
    bool      running;

    // ---- 动态对象池 ----
    std::vector<Bullet*> bullets;
    std::vector<Enemy*>  enemies;
    std::vector<Prop*>   props;

    Boss* boss;

    // ★ 贴图资源
    GameTextures textures;

    // ---- 敌机生成相关 ----
    int enemySpawnTimer;
    int enemySpawnRate;

    // ---- 关卡系统 ----
    int currentLevel;
    int enemiesKilled;
    int enemiesToPass;
    bool levelTransitioning;
    bool bossDefeated;                 // ★ Boss 是否已被击败

    // ---- 结算选 Buff ----
    BuffData buffOptions[3];
    int      buffSelected;

    // 窗口默认尺寸
    static const int DEFAULT_WIDTH  = 480;
    static const int DEFAULT_HEIGHT = 720;

    // ---- 四关预设数据（含第四关 Boss）----
    static const LevelData LEVELS[4];

    // ---- 私有辅助 ----
    void ConstrainPlayerToScreen();
    bool ProcessInput();
    void Update();
    void Render();

    // Phase 2
    void PlayerShoot();
    void SpawnEnemy();
    void CheckCollisions();
    void CleanDeadObjects();

    // Phase 3
    void ProcessInputPlaying();
    void ProcessInputSettlement();
    void UpdatePlaying();
    void UpdateSettlement();
    void RenderPlaying();
    void RenderSettlement();
    void RenderMenu();
    void RenderGameOver();
    void StartLevel(int level);
    void GenerateBuffOptions();
    void ApplyBuff(int index);
    void ResetLevel();

    // Phase 4 — Boss / Prop / 特效
    void SpawnBoss();
    void UpdateBossAndProps();
    void BossSkillBlackhole();
    void BossSkillLaser();
    void BossSkillMissile();
    void SpawnPropFromEnemy(Enemy* e);
    void SpawnNaturalProp();           // ★ 自然掉落
    void CheckPropCollisions();
    void ApplyProp(Prop* p);
    void UsePlayerBomb();              // ★ 玩家主动释放炸弹
    void TriggerFlash(int r, int g, int b, int intensity, int dur);

    // Boss 专用
    void CheckBossCollisions();
    std::vector<Bullet*> bossMissiles;
    void CleanBossMissiles();

    // ★ 敌机子弹专用
    std::vector<Bullet*> enemyBullets;
    void EnemyShoot();                  // 敌机射击逻辑
    void CleanEnemyBullets();

public:
    GameManager();
    ~GameManager();

    bool Init();
    void Run();
    void Cleanup();

    int  GetWindowWidth()  const { return windowWidth; }
    int  GetWindowHeight() const { return windowHeight; }
    bool IsRunning()       const { return running; }
};
