#pragma once
#include "IGameObject.hpp"

// ─── Colour palette ────────────────────────────────────────────────────────
inline constexpr Color COL_GREEN       = {38,  185, 154, 255};
inline constexpr Color COL_DARK_GREEN  = {20,  160, 133, 255};
inline constexpr Color COL_LIGHT_GREEN = {129, 204, 184, 255};
inline constexpr Color COL_YELLOW      = {243, 213,  91, 255};

// ─── Ball ──────────────────────────────────────────────────────────────────
class Ball : public IGameObject {
public:
    float x, y;
    float speed_x, speed_y;
    int   radius;

    // Factory: always allocate through unique_ptr
    static std::unique_ptr<Ball> Create(float x, float y,
                                        float sx, float sy, int r) {
        auto b    = std::make_unique<Ball>();
        b->x      = x;  b->y      = y;
        b->speed_x = sx; b->speed_y = sy;
        b->radius  = r;
        return b;
    }

    void Draw() const override {
        DrawCircle(static_cast<int>(x), static_cast<int>(y), radius, COL_YELLOW);
    }

    void Update() override {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0)
            speed_y *= -1.0f;

        if (x + radius >= GetScreenWidth())  { cpu_score_++;  Reset(); }
        if (x - radius <= 0)                 { player_score_++; Reset(); }
    }

    int PlayerScore() const { return player_score_; }
    int CpuScore()    const { return cpu_score_;    }

private:
    int player_score_ = 0;
    int cpu_score_    = 0;

    void Reset() {
        x = static_cast<float>(GetScreenWidth())  / 2.f;
        y = static_cast<float>(GetScreenHeight()) / 2.f;
        int choices[2] = {-1, 1};
        speed_x *= static_cast<float>(choices[GetRandomValue(0, 1)]);
        speed_y *= static_cast<float>(choices[GetRandomValue(0, 1)]);
    }
};
