#pragma once
#include "GameObject.h"

// ============================================================
// Prop 类型
// ============================================================
enum PropType {
    PROP_HEAL,       // 回血球（击杀掉落 + 自然掉落）
    PROP_SCATTER,    // 散射技能球（仅自然掉落，10秒时效）
    PROP_BOMB,       // 全屏炸弹（仅结算/特殊获取）
    PROP_SHIELD      // 临时护盾（保留，击杀掉落）
};

// ============================================================
// Prop - 掉落物/技能球，继承自 GameObject
// ============================================================
class Prop : public GameObject {
private:
    PropType type;
    int   lifeTimer;     // 存活倒计时
    float floatOffset;
    int   floatTimer;
    float fallSpeed;     // ★ 新：下落速度（自然掉落用）

public:
    Prop(float x, float y, PropType t, float fallSpd = 0.0f);

    void Update() override;
    void Draw()   override;

    PropType GetType() const { return type; }
    int GetLifeTimer() const { return lifeTimer; }  // ★ 新
};
