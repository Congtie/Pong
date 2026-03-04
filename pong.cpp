/**
 * Pong  –  Motor Grafic C++ cu Raylib & OOP
 *
 * Caracteristici Modern C++:
 *   • Smart pointers (unique_ptr) pentru ownership explicit al obiectelor
 *   • Template GameObjectManager<T> pentru gestionarea colecțiilor polimorfice
 *   • STL: std::vector, std::array, std::all_of, std::remove_if, std::map
 *   • Moștenire & polimorfism virtual: IGameObject → Paddle → CpuPaddle
 *   • Factory methods statice (Paddle::Create, Ball::Create, Tetromino::Spawn)
 */

#include <raylib.h>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include "IGameObject.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"

// ─── Colour palette re-exported for main ──────────────────────────────────
static constexpr Color BG_COLOR    = COL_DARK_GREEN;
static constexpr Color ACCENT      = COL_GREEN;
static constexpr Color CIRCLE_COL  = COL_LIGHT_GREEN;

// ─── Helper: AABB from Paddle reference ───────────────────────────────────
static inline Rectangle PaddleRect(const Paddle& p) {
    return { p.x, p.y, p.width, p.height };
}

// ─── Game state ────────────────────────────────────────────────────────────
struct PongGame {
    std::unique_ptr<Ball>      ball;
    std::unique_ptr<Paddle>    player;
    std::unique_ptr<CpuPaddle> cpu;

    // Particle trail (STL vector of positions) for visual flair
    std::vector<Vector2> trail;
    static constexpr std::size_t TRAIL_MAX = 20;

    void Init(int sw, int sh) {
        ball   = Ball::Create(sw / 2.f, sh / 2.f, 7.f, 7.f, 20);
        player = Paddle::Create(sw - 35.f,  sh / 2.f - 60.f, 25.f, 120.f, 6.f);
        cpu    = CpuPaddle::Create(10.f,    sh / 2.f - 60.f, 25.f, 120.f, 6.f);
    }

    void Update() {
        ball->Update();
        player->Update();
        cpu->Update(ball->y);

        // Collisions
        auto checkHit = [&](Paddle& p) {
            if (CheckCollisionCircleRec({ball->x, ball->y},
                                        ball->radius, PaddleRect(p)))
                ball->speed_x *= -1.f;
        };
        checkHit(*player);
        checkHit(*cpu);

        // Trail – push current position, cap length with STL
        trail.push_back({ball->x, ball->y});
        if (trail.size() > TRAIL_MAX)
            trail.erase(trail.begin());
    }

    void Draw(int sw, int sh) const {
        ClearBackground(BG_COLOR);

        // Court markings
        DrawRectangle(sw / 2, 0, sw / 2, sh, ACCENT);
        DrawCircle(sw / 2, sh / 2, 150, CIRCLE_COL);
        DrawLine(sw / 2, 0, sw / 2, sh, WHITE);

        // Ball trail (alpha decreases with age — modern range-for + index)
        for (std::size_t i = 0; i < trail.size(); ++i) {
            float alpha = static_cast<float>(i) / TRAIL_MAX;
            Color c = COL_YELLOW;
            c.a = static_cast<unsigned char>(alpha * 180);
            DrawCircle(static_cast<int>(trail[i].x),
                       static_cast<int>(trail[i].y),
                       ball->radius * alpha, c);
        }

        ball->Draw();
        cpu->Draw();
        player->Draw();

        // Scores
        DrawText(TextFormat("%i", ball->CpuScore()),
                 sw / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", ball->PlayerScore()),
                 3 * sw / 4 - 20, 20, 80, WHITE);
    }
};

// ─── Entry point ──────────────────────────────────────────────────────────
int main() {
    constexpr int SW = 1280, SH = 800;
    InitWindow(SW, SH, "Pong  –  C++ Modern / Raylib");
    SetTargetFPS(60);

    // Game state owned on the stack; internal objects via unique_ptr
    PongGame game;
    game.Init(SW, SH);

    while (!WindowShouldClose()) {
        game.Update();
        BeginDrawing();
        game.Draw(SW, SH);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
