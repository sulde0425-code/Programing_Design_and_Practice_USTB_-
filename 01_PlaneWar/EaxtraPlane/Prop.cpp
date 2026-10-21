#include "Prop.h"
#include <graphics.h>
#include <cmath>

Prop::Prop(float x, float y, PropType t, float fallSpd)
    : GameObject(x, y, 18, 18),
      type(t), lifeTimer(600),       // 10秒存活
      floatOffset(0.0f), floatTimer(0),
      fallSpeed(fallSpd)
{
}

void Prop::Update() {
    floatTimer++;
    floatOffset = sinf((float)floatTimer * 0.08f) * 5.0f;

    // 自然下落
    y += fallSpeed;

    // 倒计时 → 消失
    lifeTimer--;
    if (lifeTimer <= 0) {
        isAlive = false;
    }

    // 飞出屏幕底部 → 消失
    if (y > 750) {
        isAlive = false;
    }
}

void Prop::Draw() {
    int cx = (int)(x + width / 2);
    int cy = (int)(y + height / 2 + floatOffset);
    
    if (texture) {
        putimage(cx - width/2, cy - height/2, width, height, texture, 0, 0);
        return;
    }
    // Fallback
    int r  = (int)(width / 2);

    switch (type) {
    case PROP_HEAL:    setfillcolor(RGB(40, 220, 80));  setlinecolor(RGB(20, 180, 50));  break;
    case PROP_SCATTER: setfillcolor(RGB(255, 200, 30)); setlinecolor(RGB(220, 160, 10)); break;
    case PROP_BOMB:    setfillcolor(RGB(255, 80, 40));  setlinecolor(RGB(220, 50, 20));  break;
    case PROP_SHIELD:  setfillcolor(RGB(60, 140, 255)); setlinecolor(RGB(30, 100, 220)); break;
    }

    fillcircle(cx, cy, r);
    setlinecolor(RGB(255, 255, 255));
    if (type == PROP_SCATTER) {
        line(cx-5,cy,cx+5,cy); line(cx,cy-5,cx,cy+5); line(cx-3,cy-3,cx+3,cy+3);
    } else {
        line(cx-5,cy,cx+5,cy); line(cx,cy-5,cx,cy+5);
    }
}
