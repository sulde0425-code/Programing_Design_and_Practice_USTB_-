#include "GameManager.h"
#include "LevelData.h"
#include <graphics.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>

// ============================================================
// 四关预设数据（含第四关 Boss）
// ============================================================
const LevelData GameManager::LEVELS[4] = {
    { 1, "First Assault",     45,  30,  2.5f, 12, false },  // 普通
    { 2, "Iron Rain",         32,  50,  3.2f, 18, false },  // 普通
    { 3, "Final Stand",       22,  80,  4.0f, 28, false },  // 普通
    { 4, "THE BOSS",          60,  60,  2.0f,  1, true  }   // ★ Boss 关
};

// ============================================================
// 构造 / 析构
// ============================================================
GameManager::GameManager()
    : windowWidth(DEFAULT_WIDTH), windowHeight(DEFAULT_HEIGHT),
      gameState(STATE_MENU), player(nullptr), running(false),
      enemySpawnTimer(0), enemySpawnRate(45),
      currentLevel(1), enemiesKilled(0), enemiesToPass(12),
      levelTransitioning(false), buffSelected(0),
      boss(nullptr), bossDefeated(false)
{
    srand((unsigned)time(nullptr));
    FlashInit();
}

GameManager::~GameManager() {
    Cleanup();
}

// ============================================================
// Init
// ============================================================
bool GameManager::Init() {
    initgraph(windowWidth, windowHeight);
    setbkcolor(RGB(15, 15, 40));
    cleardevice();

    settextcolor(RGB(255, 255, 255));
    setbkmode(TRANSPARENT);

    // ★ 加载贴图
    textures.LoadAll();

    float startX = (float)(windowWidth / 2 - 20);
    float startY = (float)(windowHeight - 80);
    player = new Player(startX, startY);
    player->SetTexture(textures.playerImg);
    if (textures.playerImg) player->SetSize(64, 64);  // 贴图已在加载时缩小至 64×64

    running = true;
    return true;
}

// ============================================================
// Run - 游戏主循环（状态机驱动）
// ============================================================
void GameManager::Run() {
    while (running) {
        if (!ProcessInput()) break;

        // 状态机——不同状态不同逻辑
        switch (gameState) {
        case STATE_MENU:
            break;  // 渲染在 Render() 中统一处理
        case STATE_PLAYING:
            UpdatePlaying();
            break;
        case STATE_SETTLEMENT:
            UpdateSettlement();
            break;
        case STATE_GAMEOVER:
            break;
        }

        Render();
        Sleep(16);
    }
}

// ============================================================
// ProcessInput - 键盘输入（按状态分流）
// ============================================================
bool GameManager::ProcessInput() {
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        running = false;
        return false;
    }

    switch (gameState) {
    // ---- 主菜单 ----
    case STATE_MENU: {
        // 按回车开始游戏
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            currentLevel = 1;
            enemiesKilled = 0;
            StartLevel(1);
            gameState = STATE_PLAYING;
        }
        break;
    }
    // ---- 游戏中 ----
    case STATE_PLAYING:
        ProcessInputPlaying();
        break;

    // ---- 结算 ----
    case STATE_SETTLEMENT:
        ProcessInputSettlement();
        break;

    // ---- 游戏结束 ----
    case STATE_GAMEOVER: {
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            // 重新开始
            delete player;
            float sx = (float)(windowWidth / 2 - 20);
            float sy = (float)(windowHeight - 80);
            player = new Player(sx, sy);
            currentLevel = 1;
            enemiesKilled = 0;
            ResetLevel();
            gameState = STATE_MENU;
        }
        break;
    }
    }

    return true;
}

// ============================================================
// ProcessInputPlaying - 游戏中的输入
// ============================================================
void GameManager::ProcessInputPlaying() {
    // ---- WASD / 方向键 ----
    float dirX = 0.0f, dirY = 0.0f;
    if (GetAsyncKeyState('W')     & 0x8000 || GetAsyncKeyState(VK_UP)    & 0x8000) dirY = -1.0f;
    if (GetAsyncKeyState('S')     & 0x8000 || GetAsyncKeyState(VK_DOWN)  & 0x8000) dirY =  1.0f;
    if (GetAsyncKeyState('A')     & 0x8000 || GetAsyncKeyState(VK_LEFT)  & 0x8000) dirX = -1.0f;
    if (GetAsyncKeyState('D')     & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000) dirX =  1.0f;

    if (dirX != 0.0f && dirY != 0.0f) {
        dirX *= 0.70710678f;
        dirY *= 0.70710678f;
    }
    player->Move(dirX, dirY);
    ConstrainPlayerToScreen();

    // ---- 射击 ----
    if (GetAsyncKeyState(VK_SPACE) & 0x8000 ||
        GetAsyncKeyState('J')      & 0x8000) {
        PlayerShoot();
    }

    // ---- 炸弹（B 键触发）----
    if (GetAsyncKeyState('B') & 0x8000) {
        UsePlayerBomb();
    }
}

// ============================================================
// ProcessInputSettlement - 结算选 Buff 的输入
// ============================================================
void GameManager::ProcessInputSettlement() {
    // 上下切换选项
    static bool upPressed = false, downPressed = false;
    bool upNow = GetAsyncKeyState('W') & 0x8000 || GetAsyncKeyState(VK_UP)    & 0x8000;
    bool dnNow = GetAsyncKeyState('S') & 0x8000 || GetAsyncKeyState(VK_DOWN)  & 0x8000;

    if (upNow && !upPressed) { buffSelected--; if (buffSelected < 0) buffSelected = 2; }
    if (dnNow && !downPressed) { buffSelected++; if (buffSelected > 2) buffSelected = 0; }
    upPressed = upNow;
    downPressed = dnNow;

    // 回车确认选择
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
        ApplyBuff(buffSelected);

        // 进入下一关或胜利
        currentLevel++;
        if (currentLevel > 4) {   // ★ 4关全通 = 胜利
            gameState = STATE_GAMEOVER;
        } else {
            StartLevel(currentLevel);
            gameState = STATE_PLAYING;
        }
    }
}

// ============================================================
// ConstrainPlayerToScreen
// ============================================================
void GameManager::ConstrainPlayerToScreen() {
    float px = player->GetX();
    float py = player->GetY();
    int   pw = player->GetWidth();
    int   ph = player->GetHeight();

    if (px < 0)                           px = 0;
    if (px + pw > (float)windowWidth)     px = (float)(windowWidth - pw);
    if (py < 0)                           py = 0;
    if (py + ph > (float)windowHeight)    py = (float)(windowHeight - ph);
    player->SetPosition(px, py);
}

// ============================================================
// PlayerShoot - 支持散射多行子弹
// ============================================================
void GameManager::PlayerShoot() {
    if (!player->CanShoot()) return;

    int   dmg  = (int)(15.0f * player->GetDamageMultiplier());
    float px   = player->GetX();
    float py   = player->GetY();
    int   pw   = player->GetWidth();
    int   rows = player->GetBulletRows();  // 1/2/3 行

    if (rows >= 3) {
        bullets.push_back(new Bullet(px + 1,          py - 14, -1.8f, -7.5f, dmg));
        bullets.push_back(new Bullet(px + pw/2 - 3,   py - 14,  0.0f, -8.0f, dmg));
        bullets.push_back(new Bullet(px + pw - 6 - 1, py - 14,  1.8f, -7.5f, dmg));
    } else if (rows >= 2) {
        bullets.push_back(new Bullet(px + 1,          py - 14, 0.0f, -8.0f, dmg));
        bullets.push_back(new Bullet(px + pw - 6 - 1, py - 14, 0.0f, -8.0f, dmg));
    } else {
        bullets.push_back(new Bullet(px + pw/2 - 3,   py - 14, 0.0f, -8.0f, dmg));
    }

    player->ResetFireTimer();
}

// ============================================================
// SpawnEnemy - 按关卡配置生成敌机
// ============================================================
void GameManager::SpawnEnemy() {
    enemySpawnTimer++;
    if (enemySpawnTimer < enemySpawnRate) return;
    enemySpawnTimer = 0;

    int margin = 36;
    float ex = (float)(margin + rand() % (windowWidth - 2 * margin));
    float ey = -36.0f;

    const LevelData& lv = LEVELS[currentLevel - 1];
    Enemy* e = new Enemy(ex, ey, 36, 36, lv.enemyHP, 0.0f, lv.enemySpeedY);
    if (textures.enemyImg) {
        e->SetTexture(textures.enemyImg);
        e->SetSize(50, 50);  // 贴图已在加载时缩小至 50×50
    }
    enemies.push_back(e);
}

// ============================================================
// UpdatePlaying - 游戏中的每帧更新（含 Boss/Prop）
// ============================================================
void GameManager::UpdatePlaying() {
    player->Update();
    FlashUpdate();  // ★ 闪烁倒计时

    const LevelData& lv = LEVELS[currentLevel - 1];

    // ---- Boss 关卡：不刷小兵，只打 Boss ----
    if (lv.isBossLevel) {
        UpdateBossAndProps();
    } else {
        // ---- 普通关卡：刷小兵 ----
        SpawnEnemy();
        for (auto* b : bullets) b->Update();
        for (auto* e : enemies) e->Update();
        
        // ★ 敌机射击
        EnemyShoot();
        for (auto* b : enemyBullets) b->Update();
    }

    // 玩家子弹（含 Boss 导弹）
    for (auto* b : bullets)      b->Update();
    for (auto* b : bossMissiles) b->Update();

    // Prop 更新
    for (auto* p : props) p->Update();

    // ★ 自然掉落（散射球/回血球）
    SpawnNaturalProp();

    CheckCollisions();
    CheckBossCollisions();       // ★ Boss 碰撞
    CheckPropCollisions();       // ★ 拾取 Prop
    CleanDeadObjects();
    CleanBossMissiles();         // ★ 清理 Boss 导弹

    // Prop 清理
    for (auto it = props.begin(); it != props.end(); ) {
        if (!(*it)->IsAlive()) { delete *it; it = props.erase(it); }
        else ++it;
    }

    // 玩家死亡
    if (!player->IsAlive()) {
        gameState = STATE_GAMEOVER;
        TriggerFlash(255, 40, 40, 180, 40);  // 红色闪烁
        return;
    }

    // Boss 关：击败 Boss 过关
    if (lv.isBossLevel && bossDefeated) {
        levelTransitioning = true;
        ResetLevel();
        CleanBossMissiles();
        if (boss) { delete boss; boss = nullptr; }
        GenerateBuffOptions();
        gameState = STATE_SETTLEMENT;
        TriggerFlash(255, 220, 50, 150, 30);  // 金色闪烁
        return;
    }

    // 普通关：消灭足够敌机过关
    if (!lv.isBossLevel && enemiesKilled >= enemiesToPass && !levelTransitioning) {
        levelTransitioning = true;
        ResetLevel();
        GenerateBuffOptions();
        gameState = STATE_SETTLEMENT;
        TriggerFlash(255, 220, 50, 150, 30);
    }
}

// ============================================================
// UpdateSettlement - 结算状态更新（Phase 3 无额外 Tick）
// ============================================================
void GameManager::UpdateSettlement() {
    // 结算页面纯静态，等待用户选择输入
}

// ============================================================
// CheckCollisions - 子弹 vs 敌机 / Boss（含吸血判定）
// ============================================================
void GameManager::CheckCollisions() {
    for (auto* bullet : bullets) {
        if (!bullet->IsAlive()) continue;
        for (auto* enemy : enemies) {
            if (!enemy->IsAlive()) continue;
            if (bullet->CheckCollision(*enemy)) {
                bullet->SetAlive(false);
                enemy->TakeDamage(bullet->GetDamage());

                // ★ 吸血判定：5% 概率回复 1 HP
                if (player->IsVampiricActive() &&
                    (rand() % 100) < (int)(player->GetVampiricChance() * 100)) {
                    player->Heal(1);
                }

                if (!enemy->IsAlive()) {
                    enemiesKilled++;
                    SpawnPropFromEnemy(enemy);
                }
                break;
            }
        }
    }

    for (auto* enemy : enemies) {
        if (!enemy->IsAlive()) continue;
        if (enemy->CheckCollision(*player)) {
            enemy->TakeDamage(999);
            if (!enemy->IsAlive()) {
                enemiesKilled++;
                SpawnPropFromEnemy(enemy);
            }
            player->TakeDamage(20);
            TriggerFlash(255, 60, 60, 120, 8);
        }
    }

    // ★ 玩家与敌机子弹碰撞
    for (auto* enemyBullet : enemyBullets) {
        if (!enemyBullet->IsAlive()) continue;
        if (enemyBullet->CheckCollision(*player)) {
            enemyBullet->SetAlive(false);
            if (!player->IsInvincible()) {
                player->TakeDamage(15);
                TriggerFlash(255, 60, 60, 100, 6);
            }
        }
    }
}

// ============================================================
// CleanDeadObjects
// ============================================================
void GameManager::CleanDeadObjects() {
    for (auto it = bullets.begin(); it != bullets.end(); ) {
        if (!(*it)->IsAlive()) { delete *it; it = bullets.erase(it); }
        else ++it;
    }
    for (auto it = enemies.begin(); it != enemies.end(); ) {
        if (!(*it)->IsAlive()) { delete *it; it = enemies.erase(it); }
        else ++it;
    }
    CleanBossMissiles();
    CleanEnemyBullets();  // ★ 清理敌机子弹
}

// ============================================================
// StartLevel - 初始化关卡（含 Boss）
// ============================================================
void GameManager::StartLevel(int level) {
    const LevelData& lv = LEVELS[level - 1];
    enemySpawnRate  = lv.enemySpawnRate;
    enemiesToPass   = lv.enemiesToPass;
    enemiesKilled   = 0;
    enemySpawnTimer = 0;
    levelTransitioning = false;
    bossDefeated = false;

    ResetLevel();
    CleanBossMissiles();

    // 清理旧 Boss
    if (boss) { delete boss; boss = nullptr; }

    // Boss 关卡：立即生成 Boss
    if (lv.isBossLevel) {
        SpawnBoss();
    }
}

// ============================================================
// ResetLevel - 清空所有动态对象
// ============================================================
void GameManager::ResetLevel() {
    for (auto* b : bullets) delete b;
    bullets.clear();
    for (auto* e : enemies) delete e;
    enemies.clear();
    for (auto* b : enemyBullets) delete b;  // ★ 清空敌机子弹
    enemyBullets.clear();
}

// ============================================================
// GenerateBuffOptions - 扩展 Buff 池
// ============================================================
void GameManager::GenerateBuffOptions() {
    static const BuffData BUFF_POOL[] = {
        // 伤害 (type=0)
        { "DMG Up I",    "+20% Damage",          0, 1.20f },
        { "DMG Up II",   "+35% Damage",          0, 1.35f },
        { "Big DMG",     "+50% Damage",          0, 1.50f },
        // 射速 (type=1)
        { "Fire Rate I", "+25% Fire Rate",       1, 1.25f },
        { "Fire Rate II","+40% Fire Rate",       1, 1.40f },
        { "Rapid Fire",  "+55% Fire Rate",       1, 1.55f },
        // 移速 (type=2)
        { "Speed Up I",  "+15% Move Speed",      2, 1.15f },
        { "Speed Up II", "+30% Move Speed",      2, 1.30f },
        // 回血 (type=3)
        { "Heal 30",     "Restore 30 HP",        3, 30.0f },
        { "Heal 50",     "Restore 50 HP",        3, 50.0f },
        // ★ 新 Buff (type=4-8)
        { "+MaxHP",      "+30 Max HP",           4, 30.0f },
        { "+Bullet Row", "+1 Bullet Row",        5, 1.0f  },
        { "+Invincible", "+1s Invincibility",    6, 60.0f },
        { "+Skill Time", "+3s Skill Duration",   7, 180.0f},
        { "Vampiric",    "5% Lifesteal",         8, 0.05f },
    };
    const int POOL_SIZE = sizeof(BUFF_POOL) / sizeof(BUFF_POOL[0]);

    for (int i = 0; i < 3; i++) {
        buffOptions[i] = BUFF_POOL[rand() % POOL_SIZE];
    }
    buffSelected = 0;
}

// ============================================================
// ApplyBuff - 扩展 Buff 应用
// ============================================================
void GameManager::ApplyBuff(int index) {
    const BuffData& b = buffOptions[index];

    switch (b.type) {
    case 0: player->SetDamageMultiplier(player->GetDamageMultiplier() * b.value); break;
    case 1: player->SetFireRateMultiplier(player->GetFireRateMultiplier() * b.value); break;
    case 2: player->SetSpeedMultiplier(player->GetSpeedMultiplier() * b.value); break;
    case 3: player->Heal((int)b.value); break;
    case 4: player->SetMaxHp(player->GetMaxHP() + (int)b.value); break;  // +MaxHP
    case 5: player->SetBulletRows(player->GetBulletRows() + (int)b.value); break;  // +Row
    case 6: player->SetInvincibleDuration(player->GetInvincibleDuration() + (int)b.value); break;
    case 7: player->SetSkillDurationBonus(player->GetSkillDurationBonus() + (int)b.value); break;
    case 8: player->SetVampiricActive(true); player->SetVampiricChance(b.value); break;
    }
}

// ============================================================
// Render - 统一渲染入口（按状态分流）
// ============================================================
void GameManager::Render() {
    BeginBatchDraw();
    cleardevice();

    switch (gameState) {
    case STATE_MENU:
        RenderMenu();
        break;
    case STATE_PLAYING:
        RenderPlaying();
        break;
    case STATE_SETTLEMENT:
        RenderSettlement();
        break;
    case STATE_GAMEOVER:
        RenderGameOver();
        break;
    }

    EndBatchDraw();
}

// ============================================================
// RenderMenu - 主菜单
// ============================================================
void GameManager::RenderMenu() {
    settextcolor(RGB(255, 255, 255));
    setbkmode(TRANSPARENT);

    // 标题
    settextstyle(36, 0, "Consolas");
    outtextxy(80, 200, "Roguelike Plane Battle");

    settextstyle(20, 0, "Consolas");
    outtextxy(120, 320, "Press ENTER to Start");

    settextstyle(16, 0, "Consolas");
    outtextxy(100, 400, "WASD / Arrows : Move");
    outtextxy(100, 425, "SPACE / J     : Shoot");
    outtextxy(100, 450, "ESC           : Quit");

    // 显示 Buff 状态
    char buf[128];
    sprintf_s(buf, "DMG: x%.2f | FireRate: x%.2f | Speed: x%.2f",
        player->GetDamageMultiplier(),
        player->GetFireRateMultiplier(),
        player->GetSpeedMultiplier());
    outtextxy(60, 500, buf);
}

// ============================================================
// RenderPlaying - 游戏画面（含 Boss / Prop / 激光 / 闪烁）
// ============================================================
void GameManager::RenderPlaying() {
    const LevelData& lv = LEVELS[currentLevel - 1];

    // 背景
    if (textures.backgroundImg) {
        // 背景贴图缩放到 480×720
        putimage(0, 0, windowWidth, windowHeight, textures.backgroundImg, 0, 0);
    }

    // HUD
    settextstyle(14, 0, "Consolas");
    char hud[256];
    if (lv.isBossLevel && boss) {
        sprintf_s(hud,
            "Lv.%d %s | HP:%d/%d | BossHP:%d/%d | Bomb:%d | Scatter:%d | Bul:%d",
            currentLevel, lv.name,
            player->GetHP(), player->GetMaxHP(),
            boss->GetHP(), boss->GetMaxHP(),
            player->GetBombCount(), player->GetScatterTime(),
            (int)bullets.size());
    } else {
        sprintf_s(hud,
            "Lv.%d %s | HP:%d/%d | Kills:%d/%d | Bomb:%d | Scat:%d | Vamp:%s | En:%d | Bul:%d",
            currentLevel, lv.name,
            player->GetHP(), player->GetMaxHP(),
            enemiesKilled, enemiesToPass,
            player->GetBombCount(),
            player->GetScatterTime(),
            player->IsVampiricActive() ? "ON" : "OFF",
            (int)enemies.size(), (int)bullets.size());
    }
    outtextxy(8, 5, hud);

    // 绘制 Boss 激光（在对象下方渲染，避免遮挡）
    if (boss && boss->IsAlive() && boss->GetCurrentSkill() == BOSS_SKILL_LASER) {
        int lx = boss->GetLaserX();
        int lw = boss->GetLaserWidth();
        // 激光发光效果（外层宽 — 半透明色模拟）
        setfillcolor(RGB(100, 80, 0));
        solidrectangle(lx - 5, 0, lx + lw + 5, windowHeight);
        // 激光核心
        setfillcolor(RGB(255, 255, 180));
        solidrectangle(lx, 0, lx + lw, windowHeight);
    }

    // 绘制 Prop（Boss 后方）
    for (auto* p : props) if (p->IsAlive()) p->Draw();

    // 绘制子弹
    for (auto* b : bullets)      if (b->IsAlive()) b->Draw();
    for (auto* b : bossMissiles) if (b->IsAlive()) b->Draw();
    for (auto* b : enemyBullets) if (b->IsAlive()) b->Draw();  // ★ 敌机子弹

    // 绘制敌机 / Boss
    for (auto* e : enemies) if (e->IsAlive()) e->Draw();
    if (boss && boss->IsAlive()) boss->Draw();

    // 黑洞引力视觉（粒子线从玩家到 Boss）
    if (boss && boss->IsAlive() && boss->GetCurrentSkill() == BOSS_SKILL_BLACKHOLE) {
        setlinecolor(RGB(150, 0, 200));
        for (int i = 0; i < 3; i++) {
            int ox = rand() % 10 - 5;
            int oy = rand() % 10 - 5;
            line((int)(player->GetX() + player->GetWidth() / 2),
                 (int)(player->GetY() + player->GetHeight() / 2),
                 (int)(boss->GetX() + boss->GetWidth() / 2 + ox),
                 (int)(boss->GetY() + boss->GetHeight() / 2 + oy));
        }
    }

    // 绘制玩家
    player->Draw();

    // 全屏闪烁特效
    FlashRender();
}

// ============================================================
// RenderSettlement - 结算选 Buff 页面（3 选 1）
// ============================================================
void GameManager::RenderSettlement() {
    settextcolor(RGB(255, 255, 255));
    setbkmode(TRANSPARENT);

    // 标题
    settextstyle(28, 0, "Consolas");
    outtextxy(100, 80, "LEVEL COMPLETE!");

    settextstyle(18, 0, "Consolas");
    outtextxy(110, 130, "Choose a Buff (W/S + ENTER)");

    // 3 个 Buff 选项卡片
    for (int i = 0; i < 3; i++) {
        int cardX = 60;
        int cardY = 180 + i * 80;
        int cardW = 360;
        int cardH = 60;

        // 卡片背景（选中高亮）
        if (i == buffSelected) {
            setfillcolor(RGB(60, 100, 200));
            settextcolor(RGB(255, 255, 100));
        } else {
            setfillcolor(RGB(40, 40, 70));
            settextcolor(RGB(200, 200, 200));
        }

        solidrectangle(cardX, cardY, cardX + cardW, cardY + cardH);

        // 卡片文字
        settextstyle(18, 0, "Consolas");
        outtextxy(cardX + 15, cardY + 10, buffOptions[i].name);

        settextstyle(14, 0, "Consolas");
        outtextxy(cardX + 15, cardY + 35, buffOptions[i].desc);
    }

    // 底部提示
    settextstyle(14, 0, "Consolas");
    settextcolor(RGB(180, 180, 180));
    outtextxy(130, 460, "ENTER: Confirm | ESC: Quit");

    // 展示当前 Buff 状态
    char buf[128];
    sprintf_s(buf, "Current: DMG x%.2f | FR x%.2f | SPD x%.2f",
        player->GetDamageMultiplier(),
        player->GetFireRateMultiplier(),
        player->GetSpeedMultiplier());
    outtextxy(40, 500, buf);
}

// ============================================================
// RenderGameOver - 游戏结束
// ============================================================
void GameManager::RenderGameOver() {
    setbkmode(TRANSPARENT);

    if (currentLevel > 4) {   // ★ 4关全通 = 胜利
        // 全部通关
        settextcolor(RGB(255, 220, 50));
        settextstyle(32, 0, "Consolas");
        outtextxy(110, 200, "VICTORY!");

        settextstyle(16, 0, "Consolas");
        outtextxy(60, 280, "You conquered the Boss!");

        // 最终 Buff 统计
        char buf[128];
        sprintf_s(buf, "Final Stats: DMG x%.2f | FR x%.2f | SPD x%.2f",
            player->GetDamageMultiplier(),
            player->GetFireRateMultiplier(),
            player->GetSpeedMultiplier());
        outtextxy(40, 340, buf);
    } else {
        // 阵亡
        settextcolor(RGB(255, 60, 60));
        settextstyle(32, 0, "Consolas");
        outtextxy(110, 200, "GAME OVER");

        settextstyle(16, 0, "Consolas");
        char buf[64];
        sprintf_s(buf, "You died at Level %d", currentLevel);
        outtextxy(140, 280, buf);
    }

    settextcolor(RGB(200, 200, 200));
    settextstyle(16, 0, "Consolas");
    outtextxy(120, 400, "ENTER: Back to Menu");
    outtextxy(140, 425, "ESC: Quit");
}

// ============================================================
// Cleanup
// ============================================================
void GameManager::Cleanup() {
    for (auto* b : bullets) delete b;
    bullets.clear();
    for (auto* e : enemies) delete e;
    enemies.clear();
    for (auto* p : props) delete p;
    props.clear();
    CleanBossMissiles();
    if (boss) { delete boss; boss = nullptr; }
    if (player) { delete player; player = nullptr; }
    closegraph();
}

// ============================================================
// ★ Phase 4 — SpawnBoss
// ============================================================
void GameManager::SpawnBoss() {
    float bx = (float)(windowWidth / 2 - 50);
    float by = -100.0f;
    boss = new Boss(bx, by);
    if (textures.bossImg) {
        boss->SetTexture(textures.bossImg);
        boss->SetSize(100, 100);  // 贴图已在加载时缩小至 100×100
    }
    bossDefeated = false;
}

// ============================================================
// ★ Phase 4 — UpdateBossAndProps
// ============================================================
void GameManager::UpdateBossAndProps() {
    if (!boss || !boss->IsAlive()) {
        if (boss && !boss->IsAlive() && !bossDefeated) {
            bossDefeated = true;
            enemiesKilled++;
            SpawnPropFromEnemy(boss);  // Boss 死亡必掉 Prop
            SpawnPropFromEnemy(boss);  // 掉两个
            TriggerFlash(255, 255, 100, 200, 30);
            return;
        }
        return;
    }

    // Boss 移动（Y 轴缓慢下移，到达位置后停止）
    if (boss->GetY() < 100.0f) {
        boss->SetPosition(boss->GetX(), boss->GetY() + 0.6f);
    }

    boss->Update();  // 技能/漂移更新

    // ---- 黑洞引力：拉玩家 ----
    if (boss->GetCurrentSkill() == BOSS_SKILL_BLACKHOLE) {
        BossSkillBlackhole();
    }

    // ---- 激光伤害 ----
    if (boss->GetCurrentSkill() == BOSS_SKILL_LASER) {
        BossSkillLaser();
    }

    // ---- 导弹发射 ----
    if (boss->GetCurrentSkill() == BOSS_SKILL_MISSILE) {
        BossSkillMissile();
    }

    // 更新 Boss 导弹
    for (auto* b : bossMissiles) b->Update();
}

// ============================================================
// ★ Phase 4 — BossSkillBlackhole：引力拉玩家
// ============================================================
void GameManager::BossSkillBlackhole() {
    if (!boss) return;
    float strength = boss->GetBlackholeStrength();
    float bx = boss->GetX() + boss->GetWidth() / 2;
    float by = boss->GetY() + boss->GetHeight() / 2;
    float px = player->GetX() + player->GetWidth() / 2;
    float py = player->GetY() + player->GetHeight() / 2;

    float dx = bx - px;
    float dy = by - py;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist > 1.0f) {
        dx = dx / dist * strength;
        dy = dy / dist * strength;
    }

    player->SetPosition(player->GetX() + dx, player->GetY() + dy);
    ConstrainPlayerToScreen();
}

// ============================================================
// ★ Phase 4 — BossSkillLaser：激光柱伤害检测
// ============================================================
void GameManager::BossSkillLaser() {
    if (!boss) return;
    int lx = boss->GetLaserX();
    int lw = boss->GetLaserWidth();
    float px = player->GetX();
    int   pw = player->GetWidth();

    // 玩家在激光范围内 → 扣血（每帧1点，轻柔持续伤害）
    if (px + pw > lx && px < lx + lw) {
        static int laserDmgTimer = 0;
        laserDmgTimer++;
        if (laserDmgTimer >= 20) {  // 每20帧扣1血
            laserDmgTimer = 0;
            player->TakeDamage(1);
            TriggerFlash(255, 200, 0, 80, 5);
        }
    }
}

// ============================================================
// ★ Phase 4 — BossSkillMissile：发射跟踪导弹
// ============================================================
void GameManager::BossSkillMissile() {
    if (!boss) return;
    // 由 Boss::SkillMissile 控制发射频率，这里每30帧创建一枚导弹
    // missileTimer 在 Boss 中管理，这里检查
    // 简化：每45帧发射一枚
    static int bossMissileTimer = 0;
    bossMissileTimer++;
    if (bossMissileTimer < 45) return;
    bossMissileTimer = 0;

    float bx = boss->GetX() + boss->GetWidth() / 2 - 4;
    float by = boss->GetY() + boss->GetHeight();

    // 计算朝向玩家的方向
    float px = player->GetX() + player->GetWidth() / 2;
    float py = player->GetY() + player->GetHeight() / 2;
    float dx = px - bx;
    float dy = py - by;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist > 0) { dx /= dist; dy /= dist; }

    bossMissiles.push_back(new Bullet(bx, by, dx * 3.5f, dy * 3.5f, 10, 8, 8));
}

// ============================================================
// SpawnPropFromEnemy - 击杀只掉回血/护盾（10%）
// ============================================================
void GameManager::SpawnPropFromEnemy(Enemy* e) {
    if (!e || !e->ShouldDropProp()) return;

    PropType type = (rand() % 100 < 60) ? PROP_HEAL : PROP_SHIELD;
    float px = e->GetX() + e->GetWidth() / 2 - 9;
    float py = e->GetY() + e->GetHeight() / 2;
    props.push_back(new Prop(px, py, type, 1.5f));
}

// ============================================================
// SpawnNaturalProp - 自然掉落散射球/回血球
// ============================================================
void GameManager::SpawnNaturalProp() {
    static int naturalTimer = 0;
    naturalTimer++;
    if (naturalTimer < 300) return;  // 每 5 秒掉落一次
    naturalTimer = 0;

    // 随机 X 位置
    float px = (float)(40 + rand() % (windowWidth - 80));
    float py = -20.0f;

    PropType type = (rand() % 100 < 60) ? PROP_SCATTER : PROP_HEAL;
    props.push_back(new Prop(px, py, type, 1.2f));
}

// ============================================================
// ★ Phase 4 — CheckPropCollisions
// ============================================================
void GameManager::CheckPropCollisions() {
    for (auto* p : props) {
        if (!p->IsAlive()) continue;
        if (p->CheckCollision(*player)) {
            ApplyProp(p);
            p->SetAlive(false);
        }
    }
}

// ============================================================
// ApplyProp - 拾取效果（含散射/回血/护盾/炸弹）
// ============================================================
void GameManager::ApplyProp(Prop* p) {
    switch (p->GetType()) {
    case PROP_HEAL:
        player->Heal(30);
        TriggerFlash(40, 220, 80, 100, 10);
        break;
    case PROP_SCATTER:
        player->ActivateScatter(600);  // 10 秒散射
        TriggerFlash(255, 200, 30, 100, 10);
        break;
    case PROP_SHIELD:
        player->Heal(15);
        TriggerFlash(60, 140, 255, 100, 10);
        break;
    case PROP_BOMB:
        for (auto* e : enemies) {
            if (e->IsAlive()) {
                e->TakeDamage(999);
                enemiesKilled++;
                SpawnPropFromEnemy(e);
            }
        }
        if (boss && boss->IsAlive()) {
            boss->TakeDamage(80);
        }
        // 清除所有敌机子弹
        for (auto* b : bossMissiles) b->SetAlive(false);
        TriggerFlash(255, 200, 50, 200, 20);
        break;
    }
}

// ============================================================
// UsePlayerBomb - 玩家主动释放炸弹
// ============================================================
void GameManager::UsePlayerBomb() {
    if (!player->UseBomb()) return;

    for (auto* e : enemies) {
        if (e->IsAlive()) { e->TakeDamage(999); enemiesKilled++; }
    }
    for (auto* b : bossMissiles) b->SetAlive(false);
    if (boss && boss->IsAlive()) boss->TakeDamage(80);
    TriggerFlash(255, 200, 50, 200, 25);
}

// ============================================================
// ★ Phase 4 — CheckBossCollisions：玩家子弹 vs Boss
// ============================================================
void GameManager::CheckBossCollisions() {
    if (!boss || !boss->IsAlive()) return;

    // 玩家子弹 vs Boss
    for (auto* bullet : bullets) {
        if (!bullet->IsAlive()) continue;
        if (bullet->CheckCollision(*boss)) {
            bullet->SetAlive(false);
            boss->TakeDamage(bullet->GetDamage());
        }
    }

    // Boss 导弹 vs 玩家
    for (auto* missile : bossMissiles) {
        if (!missile->IsAlive()) continue;
        if (missile->CheckCollision(*player)) {
            missile->SetAlive(false);
            player->TakeDamage(10);
            TriggerFlash(255, 100, 0, 100, 6);
        }
    }

    // Boss 本体 vs 玩家
    if (boss->CheckCollision(*player)) {
        player->TakeDamage(30);
        TriggerFlash(255, 40, 40, 150, 10);
    }
}

// ============================================================
// ★ Phase 4 — CleanBossMissiles
// ============================================================
void GameManager::CleanBossMissiles() {
    for (auto it = bossMissiles.begin(); it != bossMissiles.end(); ) {
        if (!(*it)->IsAlive()) { delete *it; it = bossMissiles.erase(it); }
        else ++it;
    }
}

// ============================================================
// ★ 敌机射击相关
// ============================================================
void GameManager::EnemyShoot() {
    // 每个活着的敌机检查是否应该射击
    for (auto e : enemies) {
        if (!e->IsAlive()) continue;
        if (!e->ShouldShoot()) continue;

        // 敌机从中心下方射击 1 发子弹，速度较慢
        float bulletX = e->GetX() + e->GetWidth() / 2.0f - 3;  // 子弹宽度=6
        float bulletY = e->GetY() + e->GetHeight();
        float bulletVy = 3.5f;  // 敌机子弹速度 = 3.5，玩家子弹速度 = -6

        Bullet* bullet = new Bullet(bulletX, bulletY, 0, bulletVy, 10, 6, 14);
        if (textures.bulletImg) bullet->SetTexture(textures.bulletImg);
        enemyBullets.push_back(bullet);

        e->ResetFireTimer();
    }
}

void GameManager::CleanEnemyBullets() {
    for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
        if (!(*it)->IsAlive()) { delete *it; it = enemyBullets.erase(it); }
        else ++it;
    }
}

// ============================================================
// ★ Phase 4 — TriggerFlash
// ============================================================
void GameManager::TriggerFlash(int r, int g, int b, int intensity, int dur) {
    RGBQUAD color;
    color.rgbRed   = (BYTE)r;
    color.rgbGreen = (BYTE)g;
    color.rgbBlue  = (BYTE)b;
    color.rgbReserved = 0;
    ScreenFlash(color, intensity, dur);
}
