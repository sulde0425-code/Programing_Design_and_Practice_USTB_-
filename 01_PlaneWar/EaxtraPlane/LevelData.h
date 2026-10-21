#pragma once

// ============================================================
// BuffData - Roguelike 增益选项
// 结算页面 3 选 1 时的单个选项
// ============================================================
struct BuffData {
    const char* name;        // Buff 名称
    const char* desc;        // 描述
    int   type;              // 0=伤害 1=射速 2=移速 3=回血
    float value;             // 倍率或数值
};

// ============================================================
// LevelData - 关卡配置
// 每关有不同的敌机生成速率、生命值、速度等
// ============================================================
struct LevelData {
    int   level;             // 关卡编号 (1/2/3/4)
    const char* name;        // 关卡名称
    int   enemySpawnRate;    // 敌机生成间隔（帧）
    int   enemyHP;           // 敌机基础血量
    float enemySpeedY;       // 敌机下落速度
    int   enemiesToPass;     // 消灭多少敌机过关
    bool  isBossLevel;       // 是否为 Boss 关卡（Phase 4）
};
