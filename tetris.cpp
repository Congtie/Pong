/**
 * Tetris  –  Motor Grafic C++ cu Raylib & OOP
 *
 * Caracteristici Modern C++:
 *   • unique_ptr pentru fiecare Tetromino activ + preview
 *   • std::array<std::array<Color,COLS>,ROWS> pentru board (zero heap alloc)
 *   • std::all_of  — detectare linie completă
 *   • std::map<PieceType,Color> pentru paleta de culori per tip
 *   • Polimorfism virtual: IGameObject → Tetromino / TetrisGame
 *   • Factory Tetromino::Spawn(PieceType)
 *
 * Taste:
 *   ←/→    mișcare stânga/dreapta
 *   ↑      rotire
 *   ↓      soft drop
 */

#include <raylib.h>
#include "Tetris.hpp"

int main() {
    constexpr int COLS_PX = COLS * CELL;
    constexpr int ROWS_PX = ROWS * CELL;
    constexpr int WIN_W   = COLS_PX + PANEL;
    constexpr int WIN_H   = ROWS_PX;

    InitWindow(WIN_W, WIN_H, "Tetris  –  C++ Modern / Raylib");
    SetTargetFPS(60);

    // Entire game state managed by a single unique_ptr
    auto tetris = std::make_unique<TetrisGame>();

    while (!WindowShouldClose()) {
        tetris->Update();
        BeginDrawing();
        tetris->Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
