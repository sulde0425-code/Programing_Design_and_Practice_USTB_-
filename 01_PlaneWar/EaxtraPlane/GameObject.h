#pragma once
#include <graphics.h>

class GameObject {
protected:
    float x, y;
    int   width, height;
    bool  isAlive;
    IMAGE* texture;       // ★ 贴图指针（GameManager 设置）

public:
    GameObject(float x, float y, int w, int h)
        : x(x), y(y), width(w), height(h), isAlive(true), texture(nullptr) {}

    virtual ~GameObject() = default;

    virtual void Update() = 0;
    virtual void Draw()   = 0;

    bool CheckCollision(const GameObject& other) const {
        return (x < other.x + other.width  &&
                x + width > other.x        &&
                y < other.y + other.height &&
                y + height > other.y);
    }

    // ★ 设置贴图
    void SetTexture(IMAGE* tex) { texture = tex; }
    IMAGE* GetTexture() const   { return texture; }

    float GetX()      const { return x; }
    float GetY()      const { return y; }
    int   GetWidth()  const { return width; }
    int   GetHeight() const { return height; }
    bool  IsAlive()   const { return isAlive; }

    void SetAlive(bool alive)           { isAlive = alive; }
    void SetPosition(float nx, float ny) { x = nx; y = ny; }
    void SetSize(int w, int h)          { width = w; height = h; }
};
