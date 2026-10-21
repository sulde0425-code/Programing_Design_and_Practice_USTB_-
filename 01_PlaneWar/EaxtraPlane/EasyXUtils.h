#pragma once
#include <graphics.h>

// ============================================================
// EasyX 进阶视觉效果工具
// 透明贴图 + 全屏闪烁特效
// ============================================================

// ---- 透明贴图：将 src 图像以 alpha 透明度混合到目标窗口 ----
// srcX, srcY: 源图像左上角坐标
// alpha: 透明度 (0=完全透明, 255=完全不透明)
void putimage_alpha(int dstX, int dstY, IMAGE* src,
                    int srcX, int srcY, int srcW, int srcH,
                    int alpha = 255);

// ---- PNG 贴图（带透明通道处理）----
// 自动根据 PNG 图像的 alpha 通道进行透明混合
// 避免显示黑框问题
void PutImagePNG(int dstX, int dstY, int dstW, int dstH, IMAGE* srcImg);

// ---- 全屏闪烁特效 ----
// color: 闪烁颜色 (RGB)
// intensity: 强度 (0-255)，越大越亮
// durationFrames: 持续帧数（调用后自动管理，需每帧调用）
void ScreenFlash(RGBQUAD color, int intensity, int durationFrames);

// ---- 初始化 / 每帧调用 ----
void FlashInit();          // 在 Run() 开始前调用
void FlashUpdate();        // 每帧 Update 中调用
void FlashRender(IMAGE* backBuffer = nullptr); // 每帧 Render 结束时调用
