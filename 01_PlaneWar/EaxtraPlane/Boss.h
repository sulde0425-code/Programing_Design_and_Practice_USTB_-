#pragma once
#include "Enemy.h"

// ============================================================
// Boss 技能类型
// ============================================================
enum BossSkill {
    BOSS_SKILL_NONE,
    BOSS_SKILL_BLACKHOLE,  // 黑洞引力：把玩家拉向 Boss
    BOSS_SKILL_LASER,      // 激光柱：全屏竖线伤害
    BOSS_SKILL_MISSILE     // 自瞄导弹：跟踪玩家的子弹
};

// ============================================================
// Boss - 继承 Enemy，拥有独特的三技能 + 阶段切换机制
// 每损失 1/3 血量切换阶段，释放不同技能
// ============================================================
class Boss : public Enemy {
private:
    // ---- 技能系统 ----
    BossSkill currentSkill;   // 当前释放的技能
    int skillTimer;           // 当前技能持续计时
    int skillCooldown;        // 技能冷却计时
    int phase;                // 当前阶段 (0/1/2)
    int phaseHPThreshold;     // 进入下一阶段的血量阈值

    // ---- 黑洞技能参数 ----
    float blackholePullStrength;  // 引力强度

    // ---- 激光技能参数 ----
    int   laserX;             // 激光 X 位置
    int   laserWidth;         // 激光宽度

    // ---- 导弹技能参数 ----
    int missileTimer;         // 导弹发射间隔

    // ---- 行为参数 ----
    float moveDirX;           // Boss 左右漂移方向
    int   moveTimer;          // 漂移计时
    bool  isBoss;             // 标记（用于 GameManager 识别）

public:
    Boss(float x, float y);

    void Update() override;
    void Draw()   override;

    // ---- 技能相关 ----
    bool  IsUsingSkill() const { return skillTimer > 0; }
    float GetBlackholeStrength() const { return blackholePullStrength; }
    BossSkill GetCurrentSkill() const { return currentSkill; }
    int   GetLaserX()     const { return laserX; }
    int   GetLaserWidth() const { return laserWidth; }

private:
    // ---- 阶段控制 ----
    void UpdatePhase();
    void ActivateSkill();
    void DeactivateSkill();

    // ---- 三个技能 ----
    void SkillBlackhole();
    void SkillLaser();
    void SkillMissile();
};
