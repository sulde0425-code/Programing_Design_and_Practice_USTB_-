#pragma once
#include "Aircraft.h"
#include <cstdlib>

// ============================================================
// Enemy - 普通敌机小兵，继承自 Aircraft
// 自动向下移动，被子弹击中后掉血，死亡后可能掉落 Prop
// 发射速度低于玩家的子弹
// ============================================================
class Enemy : public Aircraft {
private:
    int fireTimer;
    int fireInitCooldown;  // 射击冷却（帧数）

public:
    Enemy(float x, float y, int w = 36, int h = 36,
          int hp = 30, float spdX = 0.0f, float spdY = 2.5f);

    void Update() override;
    void Draw()   override;

    // Phase 4：掉落物相关
    virtual bool ShouldDropProp() const { return (rand() % 100 < 15); }  // 15% 掉率

    // ★ 射击能力
    bool ShouldShoot() const { return fireTimer <= 0; }
    void ResetFireTimer() { fireTimer = fireInitCooldown; }
};
