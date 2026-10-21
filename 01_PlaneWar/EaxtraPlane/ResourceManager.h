#pragma once
#include <graphics.h>

// ============================================================
// GameTextures - 贴图资源管理
// 有贴图则用贴图，无贴图自动 fallback 原始简略色块
// ============================================================
struct GameTextures {
    IMAGE* playerImg;      // 128x128
    IMAGE* enemyImg;       // 256x256
    IMAGE* bossImg;        // 132x132
    IMAGE* bulletImg;      // nullptr → 色块
    IMAGE* propHealImg;    // nullptr → 色块
    IMAGE* propScatterImg; // nullptr → 色块
    IMAGE* propShieldImg;  // nullptr → 色块
    IMAGE* propBombImg;    // nullptr → 色块
    IMAGE* backgroundImg;  // 1536x2752

    GameTextures();
    ~GameTextures();

    bool LoadAll();
    bool IsOk() const;
};
