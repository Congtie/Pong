#pragma once
#include "IGameObject.hpp"

// ─── Base Paddle ───────────────────────────────────────────────────────────
class Paddle : public IGameObject {
public:
    float x, y, width, height;
    float speed;

    static std::unique_ptr<Paddle> Create(float x, float y,
                                          float w, float h, float spd) {
        auto p    = std::make_unique<Paddle>();
        p->x      = x;  p->y      = y;
        p->width  = w;  p->height = h;
        p->speed  = spd;
        return p;
    }

    void Draw() const override {
        DrawRectangleRounded({x, y, width, height}, 0.8f, 0, WHITE);
    }

    // Polymorphic update — player uses keyboard
    void Update() override {
        if (IsKeyDown(KEY_UP))   y -= speed;
        if (IsKeyDown(KEY_DOWN)) y += speed;
        Clamp();
    }

protected:
    void Clamp() {
        if (y <= 0) y = 0;
        if (y + height >= static_cast<float>(GetScreenHeight()))
            y = static_cast<float>(GetScreenHeight()) - height;
    }
};

// ─── CPU Paddle — overrides Update with AI behaviour ──────────────────────
class CpuPaddle : public Paddle {
public:
    static std::unique_ptr<CpuPaddle> Create(float x, float y,
                                             float w, float h, float spd) {
        auto p    = std::make_unique<CpuPaddle>();
        p->x      = x;  p->y      = y;
        p->width  = w;  p->height = h;
        p->speed  = spd;
        return p;
    }

    // Hide base Update; called explicitly with ball_y
    void Update() override { /* no-op; use Update(ball_y) */ }

    void Update(float ball_y) {
        float centre = y + height / 2.f;
        if (centre > ball_y) y -= speed;
        if (centre < ball_y) y += speed;
        Clamp();
    }
};
