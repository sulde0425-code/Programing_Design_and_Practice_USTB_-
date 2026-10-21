#include "Boss.h"
#include <graphics.h>
#include <cstdlib>
#include <cmath>

// ============================================================
// 构造函数
// Boss：大尺寸 80×70，血量 500，速度较慢，左右漂移
// ============================================================
Boss::Boss(float x, float y)
    : Enemy(x, y, 80, 70, 500, 0.8f, 0.0f),  // 基础速度下调
      currentSkill(BOSS_SKILL_NONE),
      skillTimer(0), skillCooldown(0),
      phase(0), phaseHPThreshold(maxHp * 2 / 3),
      blackholePullStrength(1.8f),
      laserX(0), laserWidth(40),
      missileTimer(0),
      moveDirX(1.0f), moveTimer(0),
      isBoss(true)
{
    // Boss 初始位置：屏幕顶部中央
    // 已通过构造函数传入 x,y
}

// ============================================================
// Update - Boss 每帧更新
// ============================================================
void Boss::Update() {
    if (!isAlive) return;

    // ---- 阶段判断 ----
    UpdatePhase();

    // ---- 水平漂移 ----
    moveTimer++;
    if (moveTimer > 90) {
        moveTimer = 0;
        moveDirX = (float)((rand() % 2 == 0) ? 1.0f : -1.0f);
    }

    // 屏幕边界反弹
    if (x <= 0)          moveDirX = 1.0f;
    if (x + width >= 480) moveDirX = -1.0f;

    x += speedX * moveDirX;

    // ---- 技能系统 ----
    if (skillTimer > 0) {
        // 正在释放技能
        skillTimer--;

        if (currentSkill == BOSS_SKILL_MISSILE) {
            SkillMissile();        // 每帧尝试发射导弹
        }
        if (currentSkill == BOSS_SKILL_LASER) {
            SkillLaser();          // 激光位置跟随 Boss X
        }
        if (currentSkill == BOSS_SKILL_BLACKHOLE) {
            SkillBlackhole();      // 持续引力
        }

        if (skillTimer <= 0) {
            DeactivateSkill();
        }
    } else if (skillCooldown > 0) {
        // 冷却中
        skillCooldown--;
    } else {
        // 冷却完毕，激活新技能
        ActivateSkill();
    }
}

// ============================================================
// UpdatePhase - 根据血量切换阶段
// ============================================================
void Boss::UpdatePhase() {
    int newPhase = 0;
    if (hp <= maxHp / 3)          newPhase = 2;
    else if (hp <= maxHp * 2 / 3) newPhase = 1;

    if (newPhase != phase) {
        phase = newPhase;
        // 阶段切换时强制激活技能
        skillCooldown = 0;
        skillTimer = 0;
    }
}

// ============================================================
// ActivateSkill - 根据阶段激活对应技能
// Phase 0: 黑洞引力
// Phase 1: 激光柱
// Phase 2: 自瞄导弹
// ============================================================
void Boss::ActivateSkill() {
    switch (phase) {
    case 0:
        currentSkill = BOSS_SKILL_BLACKHOLE;
        skillTimer = 120;      // 持续 2 秒
        break;
    case 1:
        currentSkill = BOSS_SKILL_LASER;
        laserX = (int)(x + width / 2 - laserWidth / 2);
        skillTimer = 90;       // 持续 1.5 秒
        break;
    case 2:
        currentSkill = BOSS_SKILL_MISSILE;
        missileTimer = 0;
        skillTimer = 150;      // 持续 2.5 秒
        break;
    }
}

// ============================================================
// DeactivateSkill - 技能结束，进入冷却
// ============================================================
void Boss::DeactivateSkill() {
    currentSkill = BOSS_SKILL_NONE;
    skillCooldown = 60;  // 冷却 1 秒
}

// ============================================================
// SkillBlackhole - 黑洞引力（由 GameManager 读取强度拉玩家）
// ============================================================
void Boss::SkillBlackhole() {
    // 引力强度随阶段递增
    blackholePullStrength = 1.5f + phase * 0.5f;
}

// ============================================================
// SkillLaser - 激光柱 X 位置跟随 Boss
// ============================================================
void Boss::SkillLaser() {
    laserX = (int)(x + width / 2 - laserWidth / 2);
}

// ============================================================
// SkillMissile - 发射自瞄导弹（由 GameManager 创建跟踪子弹）
// ============================================================
void Boss::SkillMissile() {
    missileTimer++;
    // 每 30 帧发射一枚导弹（实际创建在 GameManager 中）
}

// ============================================================
// Draw - Boss 渲染（大红色块 + 技能特效指示器 + 血条）
// ============================================================
void Boss::Draw() {
    if (texture) {
        putimage((int)x, (int)y, width, height, texture, 0, 0);
    } else {
        setfillcolor(RGB(180, 20, 20));
        solidrectangle((int)x, (int)y, (int)(x + width), (int)(y + height));
        setfillcolor(RGB(220, 40, 40));
        solidrectangle((int)(x + 8), (int)(y + 8),
                       (int)(x + width - 8), (int)(y + height - 8));
    }

    // 技能图标
    int indY = (int)(y - 18);
    int indX = (int)(x + width / 2 - 10);
    switch (currentSkill) {
    case BOSS_SKILL_BLACKHOLE: setfillcolor(RGB(150, 0, 200)); solidrectangle(indX, indY, indX+20, indY+10); break;
    case BOSS_SKILL_LASER:     setfillcolor(RGB(255, 220, 0)); solidrectangle(indX, indY, indX+20, indY+10); break;
    case BOSS_SKILL_MISSILE:   setfillcolor(RGB(255, 120, 0)); solidrectangle(indX, indY, indX+20, indY+10); break;
    default: break;
    }

    // Boss 大血条
    int barW = width + 20, barH = 8;
    int barX = (int)(x - 10), barY = (int)(y - 32);
    setfillcolor(RGB(40, 40, 40));
    solidrectangle(barX, barY, barX + barW, barY + barH);
    float hpRatio = (float)hp / (float)maxHp;
    if (hpRatio > 1.0f) hpRatio = 1.0f;
    if      (hpRatio > 0.6f) setfillcolor(RGB(255, 60, 60));
    else if (hpRatio > 0.3f) setfillcolor(RGB(255, 180, 0));
    else                      setfillcolor(RGB(255, 40, 40));
    solidrectangle(barX, barY, barX + (int)(barW * hpRatio), barY + barH);
}
