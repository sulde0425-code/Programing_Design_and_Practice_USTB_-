#include "EasyXUtils.h"

// ============================================================
// 全局闪烁状态（由 GameManager 通过 FlashXxx 接口管理）
// ============================================================
static RGBQUAD g_flashColor = { 0, 0, 0, 0 };
static int    g_flashIntensity = 0;
static int    g_flashDuration  = 0;
static int    g_flashTimer     = 0;
static bool   g_flashActive    = false;

// ============================================================
// putimage_alpha - 透明贴图实现
// 原理：逐像素读取 src 和 dst，按 alpha 混合
// ============================================================
void putimage_alpha(int dstX, int dstY, IMAGE* src,
                    int srcX, int srcY, int srcW, int srcH,
                    int alpha)
{
    if (alpha <= 0 || src == nullptr) return;
    if (alpha > 255) alpha = 255;

    // 获取工作区（目标图像）
    IMAGE* dst = GetWorkingImage();
    if (dst == nullptr) return;

    DWORD* dstBuf = GetImageBuffer(dst);
    DWORD* srcBuf = GetImageBuffer(src);

    if (dstBuf == nullptr || srcBuf == nullptr) return;

    int dstW = dst->getwidth();
    int dstH = dst->getheight();
    int srcFullW = src->getwidth();

    for (int row = 0; row < srcH; row++) {
        int dy = dstY + row;
        if (dy < 0 || dy >= dstH) continue;

        for (int col = 0; col < srcW; col++) {
            int dx = dstX + col;
            if (dx < 0 || dx >= dstW) continue;

            int si = (srcY + row) * srcFullW + (srcX + col);
            int di = dy * dstW + dx;

            DWORD sp = srcBuf[si];
            DWORD dp = dstBuf[di];

            // 提取各通道
            int sa = (sp >> 24) & 0xFF;
            int sr = (sp >> 16) & 0xFF;
            int sg = (sp >> 8)  & 0xFF;
            int sb =  sp        & 0xFF;

            int dr = (dp >> 16) & 0xFF;
            int dg = (dp >> 8)  & 0xFF;
            int db =  dp        & 0xFF;

            // 综合 alpha
            int blendA = (sa * alpha) / 255;

            // 混合
            int rr = (sr * blendA + dr * (255 - blendA)) / 255;
            int gg = (sg * blendA + dg * (255 - blendA)) / 255;
            int bb = (sb * blendA + db * (255 - blendA)) / 255;

            dstBuf[di] = (0xFF << 24) | (rr << 16) | (gg << 8) | bb;
        }
    }
}

// ============================================================
// PutImagePNG - PNG 透明贴图（处理 alpha 通道）
// 自动处理 PNG 的透明背景，避免黑框问题
// ============================================================
void PutImagePNG(int dstX, int dstY, int dstW, int dstH, IMAGE* srcImg) {
    if (srcImg == nullptr || dstW <= 0 || dstH <= 0) return;

    IMAGE* dst = GetWorkingImage();
    if (dst == nullptr) return;

    DWORD* dstBuf = GetImageBuffer(dst);
    DWORD* srcBuf = GetImageBuffer(srcImg);

    if (dstBuf == nullptr || srcBuf == nullptr) return;

    int dstWidth = dst->getwidth();
    int dstHeight = dst->getheight();
    int srcWidth = srcImg->getwidth();
    int srcHeight = srcImg->getheight();

    if (srcWidth <= 0 || srcHeight <= 0) return;

    // 缩放比例
    float scaleX = (float)srcWidth / dstW;
    float scaleY = (float)srcHeight / dstH;

    // 遍历目标矩形，从源图像采样
    for (int dy = 0; dy < dstH; dy++) {
        int py = dstY + dy;
        if (py < 0 || py >= dstHeight) continue;

        for (int dx = 0; dx < dstW; dx++) {
            int px = dstX + dx;
            if (px < 0 || px >= dstWidth) continue;

            // 从源图像采样（最近邻）
            int sx = (int)(dx * scaleX);
            int sy = (int)(dy * scaleY);
            if (sx >= srcWidth) sx = srcWidth - 1;
            if (sy >= srcHeight) sy = srcHeight - 1;

            DWORD srcPixel = srcBuf[sy * srcWidth + sx];
            int srcAlpha = (srcPixel >> 24) & 0xFF;

            // 跳过完全透明的像素
            if (srcAlpha == 0) continue;

            DWORD& dstPixel = dstBuf[py * dstWidth + px];

            // 提取各通道
            int sr = (srcPixel >> 16) & 0xFF;
            int sg = (srcPixel >> 8) & 0xFF;
            int sb = srcPixel & 0xFF;

            int dr = (dstPixel >> 16) & 0xFF;
            int dg = (dstPixel >> 8) & 0xFF;
            int db = dstPixel & 0xFF;

            // 按 alpha 混合
            int rr = (sr * srcAlpha + dr * (255 - srcAlpha)) / 255;
            int gg = (sg * srcAlpha + dg * (255 - srcAlpha)) / 255;
            int bb = (sb * srcAlpha + db * (255 - srcAlpha)) / 255;

            dstPixel = (0xFF << 24) | (rr << 16) | (gg << 8) | bb;
        }
    }
}

// ============================================================
// ScreenFlash - 触发全屏闪烁
// ============================================================
void ScreenFlash(RGBQUAD color, int intensity, int durationFrames) {
    g_flashColor     = color;
    g_flashIntensity = intensity;
    g_flashDuration  = durationFrames;
    g_flashTimer     = durationFrames;
    g_flashActive    = true;
}

// ============================================================
// FlashInit - 初始化
// ============================================================
void FlashInit() {
    g_flashActive = false;
    g_flashTimer  = 0;
}

// ============================================================
// FlashUpdate - 每帧更新闪烁倒计时
// ============================================================
void FlashUpdate() {
    if (!g_flashActive) return;

    g_flashTimer--;
    if (g_flashTimer <= 0) {
        g_flashActive = false;
    }
}

// ============================================================
// FlashRender - 绘制闪烁覆盖层
// ============================================================
void FlashRender(IMAGE* backBuffer) {
    if (!g_flashActive) return;

    // 当前透明度 = 强度 × (剩余时间/总时间)
    float ratio = (float)g_flashTimer / (float)g_flashDuration;
    int alpha = (int)(g_flashIntensity * ratio);
    if (alpha > 255) alpha = 255;
    if (alpha <= 0) return;

    // 设置覆盖层颜色 + alpha
    COLORREF flashClr = RGB(
        g_flashColor.rgbRed,
        g_flashColor.rgbGreen,
        g_flashColor.rgbBlue
    );

    // 使用逐像素混合实现半透明闪烁
    IMAGE* dst = GetWorkingImage();
    if (dst == nullptr) return;

    DWORD* buf = GetImageBuffer(dst);
    if (buf == nullptr) return;

    int imgW = dst->getwidth();
    int imgH = dst->getheight();
    int total = imgW * imgH;

    int fr = g_flashColor.rgbRed;
    int fg = g_flashColor.rgbGreen;
    int fb = g_flashColor.rgbBlue;

    for (int i = 0; i < total; i++) {
        DWORD p = buf[i];
        int dr = (p >> 16) & 0xFF;
        int dg = (p >> 8)  & 0xFF;
        int db =  p        & 0xFF;

        int rr = (fr * alpha + dr * (255 - alpha)) / 255;
        int gg = (fg * alpha + dg * (255 - alpha)) / 255;
        int bb = (fb * alpha + db * (255 - alpha)) / 255;

        buf[i] = (0xFF << 24) | (rr << 16) | (gg << 8) | bb;
    }
}
