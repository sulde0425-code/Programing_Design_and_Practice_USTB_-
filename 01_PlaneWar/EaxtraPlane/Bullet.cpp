#include "Bullet.h"
#include <graphics.h>

// ============================================================
// 构造函数
// vx, vy: 飞行速度（像素/帧）
// dmg: 伤害值
// w, h: 子弹尺寸（默认 6×14）
// ============================================================
Bullet::Bullet(float x, float y, float vx, float vy, int dmg, int w, int h)
    : GameObject(x, y, w, h),
      damage(dmg),
      speedX(vx),
      speedY(vy)
{
}

// ============================================================
// Update - 每帧移动子弹
// 超出屏幕边界 → isAlive = false（由 GameManager 统一清理）
// ============================================================
void Bullet::Update() {
    x += speedX;
    y += speedY;

    // 超出屏幕则标记死亡（GameManager 会清理）
    if (y + height < 0 || y > 720 || x + width < 0 || x > 480) {
        isAlive = false;
    }
}

// ============================================================
// Draw - 绘制子弹
// 玩家子弹：金黄色长条
// ============================================================
void Bullet::Draw() {
    if (texture) {
        putimage((int)x, (int)y, width, height, texture, 0, 0);
    } else {
        setfillcolor(RGB(255, 220, 50));
        solidrectangle((int)x, (int)y, (int)(x + width), (int)(y + height));
    }
}
