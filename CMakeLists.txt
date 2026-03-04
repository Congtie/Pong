cmake_minimum_required(VERSION 3.18)
project(RaylibGames CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Raylib (assumed installed or provided as submodule) ───────────────────
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ── Pong ──────────────────────────────────────────────────────────────────
add_executable(pong pong.cpp)
target_link_libraries(pong PRIVATE raylib)
target_compile_options(pong PRIVATE -Wall -Wextra -O2)

# ── Tetris ────────────────────────────────────────────────────────────────
add_executable(tetris tetris.cpp)
target_link_libraries(tetris PRIVATE raylib)
target_compile_options(tetris PRIVATE -Wall -Wextra -O2)
