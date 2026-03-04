#pragma once
#include "IGameObject.hpp"
#include <array>
#include <map>
#include <optional>

// ─── Constants ─────────────────────────────────────────────────────────────
inline constexpr int CELL   = 32;   // px per grid cell
inline constexpr int COLS   = 10;
inline constexpr int ROWS   = 20;
inline constexpr int PANEL  = 180;  // right-side preview panel width

// ─── Piece shapes: each piece = 4 cells relative to origin ─────────────────
using Shape = std::array<Vector2, 4>;

enum class PieceType { I, O, T, S, Z, J, L };

// Map each type to its colour + cell offsets (spawn orientation)
inline const std::map<PieceType, Color> PIECE_COLORS {
    { PieceType::I, SKYBLUE  }, { PieceType::O, YELLOW    },
    { PieceType::T, PURPLE   }, { PieceType::S, GREEN     },
    { PieceType::Z, RED      }, { PieceType::J, BLUE      },
    { PieceType::L, ORANGE   },
};

inline Shape ShapeFor(PieceType t) {
    switch (t) {
        case PieceType::I: return {{{0,0},{1,0},{2,0},{3,0}}};
        case PieceType::O: return {{{0,0},{1,0},{0,1},{1,1}}};
        case PieceType::T: return {{{0,0},{1,0},{2,0},{1,1}}};
        case PieceType::S: return {{{1,0},{2,0},{0,1},{1,1}}};
        case PieceType::Z: return {{{0,0},{1,0},{1,1},{2,1}}};
        case PieceType::J: return {{{0,0},{0,1},{1,1},{2,1}}};
        case PieceType::L: return {{{2,0},{0,1},{1,1},{2,1}}};
    }
    return {};
}

// ─── Active falling piece ──────────────────────────────────────────────────
class Tetromino : public IGameObject {
public:
    PieceType type;
    Shape     cells;   // absolute grid coords
    Color     color;
    bool      alive = true;

    static std::unique_ptr<Tetromino> Spawn(PieceType t, int startCol = 3) {
        auto p   = std::make_unique<Tetromino>();
        p->type  = t;
        p->color = PIECE_COLORS.at(t);
        p->cells = ShapeFor(t);
        for (auto& c : p->cells) c.x += startCol;
        return p;
    }

    bool IsAlive() const override { return alive; }

    void Draw() const override {
        for (const auto& c : cells)
            DrawRectangle(static_cast<int>(c.x) * CELL,
                          static_cast<int>(c.y) * CELL,
                          CELL - 1, CELL - 1, color);
    }

    void Update() override { /* gravity handled by TetrisGame */ }

    // Try to move by (dx, dy); validate against board before committing
    bool TryMove(int dx, int dy,
                 const std::array<std::array<Color,COLS>,ROWS>& board) {
        Shape next = cells;
        for (auto& c : next) { c.x += dx; c.y += dy; }
        for (const auto& c : next) {
            if (c.x < 0 || c.x >= COLS || c.y >= ROWS) return false;
            if (c.y >= 0 && board[static_cast<int>(c.y)][static_cast<int>(c.x)].a != 0)
                return false;
        }
        cells = next;
        return true;
    }

    // Rotate 90° CW around bounding-box centre; validates against board
    bool TryRotate(const std::array<std::array<Color,COLS>,ROWS>& board) {
        // Find pivot (first cell)
        float px = cells[0].x, py = cells[0].y;
        Shape next = cells;
        for (auto& c : next) {
            float rx = c.x - px, ry = c.y - py;
            c.x = px - ry;
            c.y = py + rx;
        }
        for (const auto& c : next) {
            if (c.x < 0 || c.x >= COLS || c.y < 0 || c.y >= ROWS) return false;
            if (board[static_cast<int>(c.y)][static_cast<int>(c.x)].a != 0)  return false;
        }
        cells = next;
        return true;
    }
};

// ─── The board + game logic ────────────────────────────────────────────────
class TetrisGame : public IGameObject {
public:
    int  score   = 0;
    int  lines   = 0;
    bool gameOver = false;

    TetrisGame() {
        // Zero-fill board (transparent = empty)
        for (auto& row : board_)
            row.fill({0,0,0,0});
        SpawnNext();
    }

    void Draw() const override {
        // Grid background
        DrawRectangle(0, 0, COLS * CELL, ROWS * CELL, {30,30,46,255});
        // Locked cells
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (board_[r][c].a != 0)
                    DrawRectangle(c*CELL, r*CELL, CELL-1, CELL-1, board_[r][c]);
        // Active piece
        if (active_) active_->Draw();
        // Ghost piece
        DrawGhost();
        // Panel
        int px = COLS * CELL + 10;
        DrawText("SCORE", px, 20,  20, WHITE);
        DrawText(TextFormat("%d", score), px, 45, 26, YELLOW);
        DrawText("LINES", px, 90,  20, WHITE);
        DrawText(TextFormat("%d", lines), px, 115, 26, SKYBLUE);
        DrawText("NEXT",  px, 160, 20, WHITE);
        if (next_) {
            // preview offset
            for (const auto& c : next_->cells)
                DrawRectangle(px + static_cast<int>(c.x)*22,
                              190 + static_cast<int>(c.y)*22,
                              21, 21, next_->color);
        }
        if (gameOver)
            DrawText("GAME OVER", COLS*CELL/2 - 80, ROWS*CELL/2 - 20, 36, RED);
    }

    void Update() override {
        if (gameOver) return;

        // ── Input ──────────────────────────────────────────────────────────
        if (IsKeyPressed(KEY_LEFT))  active_->TryMove(-1, 0, board_);
        if (IsKeyPressed(KEY_RIGHT)) active_->TryMove( 1, 0, board_);
        if (IsKeyPressed(KEY_UP))    active_->TryRotate(board_);
        if (IsKeyDown(KEY_DOWN))     softDrop_ = true;
        else                         softDrop_ = false;

        // ── Gravity ────────────────────────────────────────────────────────
        gravTimer_++;
        int interval = softDrop_ ? 2 : 30;
        if (gravTimer_ >= interval) {
            gravTimer_ = 0;
            if (!active_->TryMove(0, 1, board_)) {
                Lock();
                ClearLines();
                SpawnNext();
            }
        }
    }

private:
    std::array<std::array<Color, COLS>, ROWS> board_;
    std::unique_ptr<Tetromino> active_;
    std::unique_ptr<Tetromino> next_;
    int  gravTimer_ = 0;
    bool softDrop_  = false;

    PieceType RandomType() {
        static const std::array<PieceType,7> types {
            PieceType::I, PieceType::O, PieceType::T,
            PieceType::S, PieceType::Z, PieceType::J, PieceType::L
        };
        return types[GetRandomValue(0, 6)];
    }

    void SpawnNext() {
        active_ = next_ ? std::move(next_) : Tetromino::Spawn(RandomType());
        next_   = Tetromino::Spawn(RandomType());
        // Check game-over condition
        for (const auto& c : active_->cells)
            if (c.y >= 0 && board_[static_cast<int>(c.y)][static_cast<int>(c.x)].a != 0)
                gameOver = true;
    }

    void Lock() {
        for (const auto& c : active_->cells)
            if (c.y >= 0)
                board_[static_cast<int>(c.y)][static_cast<int>(c.x)] = active_->color;
    }

    void ClearLines() {
        int cleared = 0;
        for (int r = ROWS - 1; r >= 0; ) {
            bool full = std::all_of(board_[r].begin(), board_[r].end(),
                                    [](const Color& c){ return c.a != 0; });
            if (full) {
                // Shift everything down using STL rotate
                for (int rr = r; rr > 0; --rr)
                    board_[rr] = board_[rr - 1];
                board_[0].fill({0,0,0,0});
                ++cleared;
            } else {
                --r;
            }
        }
        // Classic Tetris scoring
        static const int pts[5] = {0, 100, 300, 500, 800};
        score += pts[std::min(cleared, 4)];
        lines += cleared;
    }

    void DrawGhost() const {
        if (!active_) return;
        // Clone cells and drop until collision
        Shape ghost = active_->cells;
        while (true) {
            Shape next = ghost;
            for (auto& c : next) c.y += 1;
            bool ok = true;
            for (const auto& c : next)
                if (c.y >= ROWS || (c.y >= 0 &&
                    board_[static_cast<int>(c.y)][static_cast<int>(c.x)].a != 0))
                { ok = false; break; }
            if (!ok) break;
            ghost = next;
        }
        for (const auto& c : ghost)
            DrawRectangle(static_cast<int>(c.x)*CELL,
                          static_cast<int>(c.y)*CELL,
                          CELL-1, CELL-1,
                          {active_->color.r, active_->color.g,
                           active_->color.b, 60});
    }
};
