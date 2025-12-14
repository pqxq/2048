#pragma once
#include "Game.hpp"
#include "raylib.h"
#include <unordered_map>

struct CellStyle {
    Color fontColor;
    Color cellColor;
    int fontSize;
};

class Renderer {
public:
    Renderer();
    void Draw(const Game& game);

private:
    void DrawBoard(const Game::Grid& grid);
    void DrawCell(int row, int col, int value);
    void DrawUI(int score, int bestScore);
    void DrawGameOver();

    std::unordered_map<int, CellStyle> m_styles;
    Camera2D m_camera;
};