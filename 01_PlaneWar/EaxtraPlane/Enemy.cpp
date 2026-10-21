#include "Enemy.h"
#include <graphics.h>

// ============================================================
// 构造函数
// ============================================================
Enemy::Enemy(float x, float y, int w, int h,
             int hp, float spdX, float spdY)
    : Aircraft(x, y, w, h, hp, spdX, spdY),
      fireTimer(0), fireInitCooldown(60)  // 敌机 60 帧冷却 ~1秒，低于玩家 8 帧
{
}

// ============================================================
// Update - 每帧自动向下移动
// 飞出屏幕底部 → isAlive = false（GameManager 统一清理）
// ============================================================
void Enemy::Update() {
    x += speedX;
    y += speedY;

    // 射击计时
    if (fireTimer > 0) fireTimer--;

    // 飞出屏幕底部则标记死亡
    if (y > 720 + height) {
        isAlive = false;
    }
}

// ============================================================
// Draw - 绘制敌机
// 橙红色矩形 + 顶部血条
// ============================================================
void Enemy::Draw() {
    if (texture) {
        putimage((int)x, (int)y, width, height, texture, 0, 0);
    } else {
        setfillcolor(RGB(220, 50, 50));
        solidrectangle((int)x, (int)y, (int)(x + width), (int)(y + height));
        setfillcolor(RGB(255, 80, 80));
        solidrectangle((int)(x + 3), (int)(y + 3),
                       (int)(x + width - 3), (int)(y + height - 3));
    }

    // 血条
    int barW = width, barH = 4;
    int barX = (int)x, barY = (int)(y - 6);
    setfillcolor(RGB(40, 40, 40));
    solidrectangle(barX, barY, barX + barW, barY + barH);
    float hpRatio = (float)hp / (float)maxHp;
    if (hpRatio > 1.0f) hpRatio = 1.0f;
    setfillcolor(RGB(255, 60, 60));
    solidrectangle(barX, barY, barX + (int)(barW * hpRatio), barY + barH);
}
