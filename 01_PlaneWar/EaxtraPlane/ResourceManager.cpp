#include "ResourceManager.h"
#include <cstdio>

// 最近邻采样缩放图像（等比例缩小）
static IMAGE* ScaleImage(IMAGE* src, int targetWidth, int targetHeight) {
    if (!src || src->getwidth() <= 0 || src->getheight() <= 0) return nullptr;
    
    int srcW = src->getwidth();
    int srcH = src->getheight();
    
    IMAGE* dst = new IMAGE(targetWidth, targetHeight);
    DWORD* srcBuf = GetImageBuffer(src);
    DWORD* dstBuf = GetImageBuffer(dst);
    
    if (!srcBuf || !dstBuf) {
        delete dst;
        return nullptr;
    }
    
    // 最近邻采样
    for (int y = 0; y < targetHeight; y++) {
        for (int x = 0; x < targetWidth; x++) {
            int srcX = x * srcW / targetWidth;
            int srcY = y * srcH / targetHeight;
            dstBuf[y * targetWidth + x] = srcBuf[srcY * srcW + srcX];
        }
    }
    
    return dst;
}

// 多路径尝试加载
static void LoadSafe(IMAGE*& img, const char* filename) {
    img = new IMAGE();
    const char* tries[] = {
        filename,
        "../assets/",
        "../../assets/",
        "../../../assets/"
    };
    for (int i = 0; i < 4; i++) {
        char buf[256];
        if (i == 0) sprintf_s(buf, "%s", filename);
        else sprintf_s(buf, "%s%s", tries[i], filename + 7);
        if (loadimage(img, buf) == 0) return;  // ★ loadimage 返回 0 = 成功
    }
    delete img; img = nullptr;
}

GameTextures::GameTextures()
    : playerImg(nullptr), enemyImg(nullptr), bossImg(nullptr),
      bulletImg(nullptr),
      propHealImg(nullptr), propScatterImg(nullptr),
      propShieldImg(nullptr), propBombImg(nullptr),
      backgroundImg(nullptr) {}

GameTextures::~GameTextures() {
    delete playerImg;      playerImg=nullptr;
    delete enemyImg;       enemyImg=nullptr;
    delete bossImg;        bossImg=nullptr;
    delete bulletImg;      bulletImg=nullptr;
    delete propHealImg;    propHealImg=nullptr;
    delete propScatterImg; propScatterImg=nullptr;
    delete propShieldImg;  propShieldImg=nullptr;
    delete propBombImg;    propBombImg=nullptr;
    delete backgroundImg;  backgroundImg=nullptr;
}

bool GameTextures::LoadAll() {
    // 仅加载存在的贴图
    LoadSafe(playerImg,     "assets/player.png");
    LoadSafe(enemyImg,      "assets/enemy.png");
    LoadSafe(bossImg,       "assets/boss.png");
    LoadSafe(backgroundImg, "assets/background.png");
    
    // ★ 玩家贴图等比例缩小（128×128 → 64×64）
    if (playerImg) {
        IMAGE* scaledPlayer = ScaleImage(playerImg, 64, 64);
        if (scaledPlayer) {
            delete playerImg;
            playerImg = scaledPlayer;
        }
    }
    
    // ★ 敌机贴图等比例缩小（256×256 → 50×50）
    if (enemyImg) {
        IMAGE* scaledEnemy = ScaleImage(enemyImg, 50, 50);
        if (scaledEnemy) {
            delete enemyImg;
            enemyImg = scaledEnemy;
        }
    }
    
    // ★ Boss贴图等比例缩小（132×132 → 100×100）
    if (bossImg) {
        IMAGE* scaledBoss = ScaleImage(bossImg, 100, 100);
        if (scaledBoss) {
            delete bossImg;
            bossImg = scaledBoss;
        }
    }
    
    return IsOk();
}

bool GameTextures::IsOk() const {
    return playerImg && playerImg->getwidth() > 0;
}
