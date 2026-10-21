#pragma once
#include "GameObject.h"

// ============================================================
// Aircraft - 飞行器基类，继承自 GameObject
// 增加生命值、速度等属性
// ============================================================
class Aircraft : public GameObject {
protected:
    int   hp;       // 当前生命值
    int   maxHp;    // 最大生命值
    float speedX;   // X 轴速度（像素/帧）
    float speedY;   // Y 轴速度（像素/帧）

public:
    Aircraft(float x, float y, int w, int h,
             int hp, float spdX, float spdY)
        : GameObject(x, y, w, h),
          hp(hp), maxHp(hp),
          speedX(spdX), speedY(spdY) {}

    virtual ~Aircraft() = default;

    // ---- 受伤 ----
    virtual void TakeDamage(int damage) {
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            isAlive = false;
        }
    }

    // ---- Getter ----
    int   GetHP()    const { return hp; }
    int   GetMaxHP() const { return maxHp; }
    float GetSpeedX() const { return speedX; }
    float GetSpeedY() const { return speedY; }
};
