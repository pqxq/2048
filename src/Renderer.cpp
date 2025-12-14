#include "Renderer.hpp"
#include "Constants.hpp"
#include <string>
#include <algorithm>
#include <iostream>

static void DrawTextCentered(const std::string& text, float x, float y, float w, float h, int fontSize, Color color) {
    int textWidth = MeasureText(text.c_str(), fontSize);
    DrawText(text.c_str(), x + w / 2 - textWidth / 2, y + h / 2 - fontSize / 2, fontSize, color);
}

Renderer::Renderer() {
    m_styles[2]    = { Config::Colors::TextDark,  Config::Hex(238, 228, 218), 60 };
    m_styles[4]    = { Config::Colors::TextDark,  Config::Hex(237, 224, 200), 60 };
    m_styles[8]    = { Config::Colors::TextLight, Config::Hex(242, 177, 121), 60 };
    m_styles[16]   = { Config::Colors::TextLight, Config::Hex(245, 149, 99),  50 };
    m_styles[32]   = { Config::Colors::TextLight, Config::Hex(246, 124, 96),  50 };
    m_styles[64]   = { Config::Colors::TextLight, Config::Hex(246, 94, 59),  50 };
    m_styles[128]  = { Config::Colors::TextLight, Config::Hex(237, 207, 115), 40 };
    m_styles[256]  = { Config::Colors::TextLight, Config::Hex(237, 204, 98),  40 };
    m_styles[512]  = { Config::Colors::TextLight, Config::Hex(237, 200, 80),  40 };
    m_styles[1024] = { Config::Colors::TextLight, Config::Hex(237, 197, 63),  30 };
    m_styles[2048] = { Config::Colors::TextLight, Config::Hex(237, 194, 45),  30 };

    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;
    m_camera.offset = {0, 0};
    m_camera.target = {0, 0};
}

void Renderer::Draw(const Game& game) {
    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());

    float scaleX = screenW / Config::DesignWidth;
    float scaleY = screenH / Config::DesignHeight;
    float scale = std::min(scaleX, scaleY);

    m_camera.zoom = scale;

    m_camera.offset.x = (screenW - (Config::DesignWidth * scale)) * 0.5f;
    m_camera.offset.y = (screenH - (Config::DesignHeight * scale)) * 0.5f;

    BeginDrawing();
    ClearBackground(Config::Colors::Background);

    BeginMode2D(m_camera);
        
        DrawRectangle(0, 0, Config::DesignWidth, Config::DesignHeight, Config::Colors::Background);

        DrawUI(game.GetScore(), game.GetBestScore());
        DrawBoard(game.GetGrid());

        if (game.IsGameOver()) {
            DrawGameOver();
        }

    EndMode2D();
    EndDrawing();
}

void Renderer::DrawUI(int score, int bestScore) {
    DrawTextCentered("2048", 0, 0, Config::DesignWidth, 225, 130, Config::Colors::TextDark);
    DrawTextCentered("Press R to restart", 0, 750, Config::DesignWidth, 50, 20, LIGHTGRAY);

    float w = 150, h = 70;
    float y = 225;
    float xCur = 50, xBest = 250;

    DrawRectangleRounded({xCur, y, w, h}, 0.1, 0, Config::Colors::Board);
    DrawRectangleRounded({xBest, y, w, h}, 0.1, 0, Config::Colors::Board);
    
    DrawTextCentered(std::to_string(score), xCur, y + 10, w, h + 5, 30, Config::Colors::TextLight);
    DrawTextCentered(std::to_string(bestScore), xBest, y + 10, w, h + 5, 30, Config::Colors::TextLight);
    DrawTextCentered("SCORE", xCur, y - 20, w, h + 5, 20, Config::Colors::EmptyCell);
    DrawTextCentered("BEST", xBest, y - 20, w, h + 5, 20, Config::Colors::EmptyCell);
}

void Renderer::DrawBoard(const Game::Grid& grid) {
    DrawRectangleRounded(
        {(float)Config::BoardOffsetX, (float)Config::BoardOffsetY, 400, 400}, 
        0.05, 0, Config::Colors::Board
    );

    for (int i = 0; i < Config::GridSize; ++i)
        for (int j = 0; j < Config::GridSize; ++j)
            DrawCell(i, j, grid[i][j]);
}

void Renderer::DrawCell(int row, int col, int value) {
    float posX = Config::BoardOffsetX + Config::Step * (col + 1) + Config::CellSize * col;
    float posY = Config::BoardOffsetY + Config::Step * (row + 1) + Config::CellSize * row;

    const auto& info = (m_styles.count(value)) ? m_styles.at(value) : m_styles[2048];
    Color bg = (value == 0) ? Config::Colors::EmptyCell : info.cellColor;

    DrawRectangleRounded({posX, posY, (float)Config::CellSize, (float)Config::CellSize}, 0.05, 0, bg);

    if (value != 0)
        DrawTextCentered(std::to_string(value), posX, posY, Config::CellSize, Config::CellSize, info.fontSize, info.fontColor);
}

void Renderer::DrawGameOver() {
    Color overlayColor = Fade(Config::Colors::Background, 0.5f);
    DrawRectangleRounded(
        {(float)Config::BoardOffsetX, (float)Config::BoardOffsetY, 400, 400}, 
        0.05, 0, overlayColor
    );
    DrawTextCentered("Game Over!", Config::BoardOffsetX, Config::BoardOffsetY, 400, 400, 50, RED);
}