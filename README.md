# Motor Grafic C++ — Raylib & OOP

Două jocuri complete construite cu **Raylib** și tehnici **C++ Modern**.

## Structura proiectului

```
src/
├── IGameObject.hpp   # Interfață abstractă + GameObjectManager<T> template
├── Ball.hpp          # Mingea Pong cu factory method
├── Paddle.hpp        # Paddle (jucător) + CpuPaddle (AI) — moștenire & polimorfism
├── Tetris.hpp        # Tetromino + TetrisGame — board STL, ghost piece, scoring
├── pong.cpp          # Entry point Pong
├── tetris.cpp        # Entry point Tetris
└── CMakeLists.txt    # Build system (C++17, FetchContent pentru Raylib)
```

## Caracteristici C++ Modern folosite

| Concept | Utilizare |
|---|---|
| `std::unique_ptr` | Ownership exclusiv al Ball, Paddle, CpuPaddle, Tetromino, TetrisGame |
| `std::make_unique` | Factory methods: `Ball::Create`, `Paddle::Create`, `Tetromino::Spawn` |
| Template `GameObjectManager<T>` | Container generic pentru orice `IGameObject`; validat la compile-time cu `static_assert` |
| `std::vector<Vector2>` | Trail vizual al mingii (Pong) |
| `std::array<std::array<Color,COLS>,ROWS>` | Board Tetris fără alocare pe heap |
| `std::all_of` | Detectare linie completă în Tetris |
| `std::remove_if` | Eliminarea obiectelor moarte din GameObjectManager |
| `std::map<PieceType,Color>` | Paleta de culori per tip de piesă |
| Virtual + override | `IGameObject::Draw()`, `IGameObject::Update()` → Paddle, CpuPaddle, Ball, Tetromino |
| `constexpr` + `inline` | Constante de culoare și dimensiuni fără overhead runtime |

## Build

```bash
cmake -B build -S .
cmake --build build
./build/pong
./build/tetris
```

## Taste

**Pong:** `↑` / `↓` — mișcare paletă jucător  
**Tetris:** `←` `→` mișcare, `↑` rotire, `↓` soft drop
