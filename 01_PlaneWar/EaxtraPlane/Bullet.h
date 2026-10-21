#pragma once
#include "GameObject.h"

// ============================================================
// Bullet - 子弹类，继承自 GameObject
// 具有伤害值、飞行方向和速度，超出屏幕自动销毁
// ============================================================
class Bullet : public GameObject {
private:
    int   damage;   // 伤害值
    float speedX;   // X 方向速度（像素/帧）
    float speedY;   // Y 方向速度（像素/帧）

public:
    // ownerIsPlayer: true=玩家子弹（向上），false=敌机子弹（向下）
    Bullet(float x, float y, float vx, float vy, int dmg, int w = 6, int h = 14);

    void Update() override;
    void Draw()   override;

    int GetDamage() const { return damage; }
};
