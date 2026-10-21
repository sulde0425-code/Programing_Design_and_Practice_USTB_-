#include "Player.h"
#include <graphics.h>

// ============================================================
// 构造
// ============================================================
Player::Player(float x, float y)
    : Aircraft(x, y, 40, 40, 100, 5.0f, 5.0f),
      fireCooldown(8), fireTimer(0),
      invincibleTimer(0), invincibleDuration(120),  // 2秒无敌
      scatterActive(false), scatterTimer(0), scatterDuration(600), // 10秒散射
      bombCount(0),
      damageMultiplier(1.0f), fireRateMultiplier(1.0f), speedMultiplier(1.0f),
      bulletRows(1), skillDurationBonus(0),
      vampiricActive(false), vampiricChance(0.05f)
{
}

// ============================================================
// Update - 所有计时器倒计时
// ============================================================
void Player::Update() {
    if (fireTimer      > 0) fireTimer--;
    if (invincibleTimer > 0) invincibleTimer--;
    if (scatterTimer   > 0) {
        scatterTimer--;
        if (scatterTimer <= 0) scatterActive = false;
    }
}

// ============================================================
// TakeDamage - 含无敌判定
// ============================================================
void Player::TakeDamage(int damage) {
    if (invincibleTimer > 0) return;  // 无敌中
    hp -= damage;
    if (hp <= 0) { hp = 0; isAlive = false; }
    else {
        // 受伤后进入无敌
        invincibleTimer = invincibleDuration + skillDurationBonus;
    }
}

// ============================================================
// ResetFireTimer
// ============================================================
void Player::ResetFireTimer() {
    float cd = (float)fireCooldown / fireRateMultiplier;
    if (cd < 1.0f) cd = 1.0f;
    fireTimer = (int)cd;
}

// ============================================================
// Heal
// ============================================================
void Player::Heal(int amount) {
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}

// ============================================================
// ActivateScatter
// ============================================================
void Player::ActivateScatter(int duration) {
    scatterActive = true;
    scatterTimer  = duration + skillDurationBonus;
}

// ============================================================
// UseBomb
// ============================================================
bool Player::UseBomb() {
    if (bombCount <= 0) return false;
    bombCount--;
    return true;
}

// ============================================================
// SetMaxHp - 修改最大HP并保持比例
// ============================================================
void Player::SetMaxHp(int newMax) {
    if (newMax <= 0) return;
    float ratio = (float)hp / (float)maxHp;
    maxHp = newMax;
    hp = (int)(maxHp * ratio);
    if (hp < 1) hp = 1;
}

// ============================================================
// Draw - 含无敌闪烁效果
// ============================================================
void Player::Draw() {
    // 无敌闪烁：每6帧切换显隐
    bool skipDraw = (invincibleTimer > 0 && (invincibleTimer / 6) % 2 == 0);

    if (!skipDraw && texture) {
        // ★ 用标准 putimage 绘制贴图（EasyX 会正确处理 PNG alpha）
        putimage((int)x, (int)y, width, height, texture, 0, 0);
    } else if (!skipDraw) {
        // 无贴图时 fallback 色块
        setfillcolor(RGB(0, 210, 255));
        solidrectangle((int)x, (int)y, (int)(x + width), (int)(y + height));
    }

    // 散射边框
    if (scatterActive) {
        setlinecolor(RGB(255, 200, 30));
        rectangle((int)x - 2, (int)y - 2,
                  (int)(x + width + 2), (int)(y + height + 2));
    }

    // 血条
    int barW = width, barH = 5;
    int barX = (int)x, barY = (int)(y - 8);
    setfillcolor(RGB(60, 60, 60));
    solidrectangle(barX, barY, barX + barW, barY + barH);
    float hpRatio = (float)hp / (float)maxHp;
    if (hpRatio > 1.0f) hpRatio = 1.0f;
    int filledW = (int)(barW * hpRatio);
    if      (hpRatio > 0.5f)  setfillcolor(RGB(0, 220, 80));
    else if (hpRatio > 0.25f) setfillcolor(RGB(255, 200, 0));
    else                       setfillcolor(RGB(255, 40, 40));
    solidrectangle(barX, barY, barX + filledW, barY + barH);
}

// ============================================================
// Move
// ============================================================
void Player::Move(float dirX, float dirY) {
    x += dirX * speedX * speedMultiplier;
    y += dirY * speedY * speedMultiplier;
}
