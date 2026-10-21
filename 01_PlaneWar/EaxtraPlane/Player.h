#pragma once
#include "Aircraft.h"

// ============================================================
// Player - 玩家飞机
// ============================================================
class Player : public Aircraft {
private:
    // ---- 射击冷却 ----
    int fireCooldown;
    int fireTimer;

    // ==== 受击无敌 ====
    int invincibleTimer;        // 当前无敌剩余帧数
    int invincibleDuration;     // 基础无敌时长（被击中后持续）

    // ==== 散射技能 ====
    bool scatterActive;         // 散射是否激活
    int  scatterTimer;          // 散射剩余帧数
    int  scatterDuration;       // 基础散射时长

    // ==== 炸弹 ====
    int  bombCount;             // 炸弹持有数

    // ==== Roguelike 增益系数 ====
    float damageMultiplier;     // 伤害倍率
    float fireRateMultiplier;   // 射速倍率
    float speedMultiplier;      // 移速倍率
    int   bulletRows;           // 子弹行数 (1/2/3)
    int   skillDurationBonus;   // 技能时长加成（帧）
    bool  vampiricActive;       // 吸血激活
    float vampiricChance;       // 吸血概率

public:
    Player(float x, float y);

    // ---- 重写虚函数 ----
    void Update() override;
    void Draw()   override;

    // ---- 重写 TakeDamage（加入无敌判定）----
    void TakeDamage(int damage) override;

    // ---- 移动 ----
    void Move(float dirX, float dirY);

    // ---- 射击 ----
    bool CanShoot()   const { return fireTimer <= 0; }
    void ResetFireTimer();

    // ---- 回血 ----
    void Heal(int amount);

    // ---- 无敌 ----
    bool IsInvincible()     const { return invincibleTimer > 0; }
    int  GetInvincibleTime() const { return invincibleTimer; }
    void SetInvincibleDuration(int d) { invincibleDuration = d; }
    int  GetInvincibleDuration() const { return invincibleDuration; }

    // ---- 散射 ----
    bool IsScatterActive() const { return scatterActive; }
    void ActivateScatter(int duration);  // 激活散射
    int  GetScatterTime()  const { return scatterTimer; }

    // ---- 炸弹 ----
    int  GetBombCount() const { return bombCount; }
    void AddBomb(int n)      { bombCount += n; }
    bool UseBomb();           // 使用一个炸弹，返回是否成功

    // ---- Getter ----
    float GetDamageMultiplier()  const { return damageMultiplier; }
    float GetFireRateMultiplier() const { return fireRateMultiplier; }
    float GetSpeedMultiplier()   const { return speedMultiplier; }
    int   GetFireCooldown()      const { return fireCooldown; }
    int   GetBulletRows()        const { return bulletRows; }
    int   GetSkillDurationBonus()const { return skillDurationBonus; }
    bool  IsVampiricActive()     const { return vampiricActive; }
    float GetVampiricChance()    const { return vampiricChance; }

    // ---- Setter ----
    void SetDamageMultiplier(float m)  { damageMultiplier = m; }
    void SetFireRateMultiplier(float m) { fireRateMultiplier = m; }
    void SetSpeedMultiplier(float m)   { speedMultiplier = m; }
    void SetBulletRows(int n)          { bulletRows = n; }
    void SetSkillDurationBonus(int n)  { skillDurationBonus = n; }
    void SetVampiricActive(bool a)     { vampiricActive = a; }
    void SetVampiricChance(float c)    { vampiricChance = c; }
    void SetMaxHp(int newMax);         // 修改最大HP（保留当前HP比例）
};
