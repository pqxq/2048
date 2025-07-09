#include "raylib.h"
#include "resource_dir.h"
#include <utility>
#include <string>
#include <unordered_map>
#include <random>
#include <chrono>
#include <array>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

constexpr int GridSize = 4;
constexpr int CellSize = 85;
constexpr int Step = 12;
constexpr int ScreenWidth = 450;
constexpr int ScreenHeight = 800;
constexpr int BoardOffsetX = 25;
constexpr int BoardOffsetY = 350;

constexpr Color MakeColor(unsigned char r, unsigned char g, unsigned char b) {
    return (Color){r, g, b, 255};
}

constexpr Color BackgroundColor = MakeColor(251, 248, 240);
constexpr Color PlaygroundColor = MakeColor(187, 173, 160);
constexpr Color EmptyCellColor = MakeColor(205, 192, 180);
constexpr Color Color2and4 = MakeColor(119, 110, 101);
constexpr Color Color8to2048 = MakeColor(249, 246, 242);
constexpr Color ColorCell2 = MakeColor(238, 228, 218);
constexpr Color ColorCell4 = MakeColor(237, 224, 200);
constexpr Color ColorCell8 = MakeColor(242, 177, 121);
constexpr Color ColorCell16 = MakeColor(245, 149, 99);
constexpr Color ColorCell32 = MakeColor(246, 124, 96);
constexpr Color ColorCell64 = MakeColor(246, 94, 59);
constexpr Color ColorCell128 = MakeColor(237, 207, 115);
constexpr Color ColorCell256 = MakeColor(237, 204, 98);
constexpr Color ColorCell512 = MakeColor(237, 200, 80);
constexpr Color ColorCell1024 = MakeColor(237, 197, 63);
constexpr Color ColorCell2048 = MakeColor(237, 194, 45);

struct CellInfo {
    Color FontCol;
    Color CellCol;
    int FontSz;
};

std::unordered_map<int, CellInfo> cellMap{
    {0, {EmptyCellColor, EmptyCellColor, 0}},
    {2, {Color2and4, ColorCell2, 60}},
    {4, {Color2and4, ColorCell4, 60}},
    {8, {Color8to2048, ColorCell8, 60}},
    {16, {Color8to2048, ColorCell16, 50}},
    {32, {Color8to2048, ColorCell32, 50}},
    {64, {Color8to2048, ColorCell64, 50}},
    {128, {Color8to2048, ColorCell128, 40}},
    {256, {Color8to2048, ColorCell256, 40}},
    {512, {Color8to2048, ColorCell512, 40}},
    {1024, {Color8to2048, ColorCell1024, 30}},
    {2048, {Color8to2048, ColorCell2048, 30}}
};

std::array<std::array<int, GridSize>, GridSize> cell;

int FreeCells = GridSize * GridSize;
int CurScore = 0;
int BestScore = 0;

std::random_device rd;
std::mt19937 gen(rd());

void SaveBestScore() {
    std::ofstream out("bestscore.dat");
    out << BestScore;
}

void LoadBestScore() {
    std::ifstream in("bestscore.dat");
    if (!(in >> BestScore)) BestScore = 0;
}

void RandomCell() {
    if (FreeCells == 0) return;

    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < GridSize; ++i)
        for (int j = 0; j < GridSize; ++j)
            if (cell[i][j] == 0) emptyCells.emplace_back(i, j);

    if (emptyCells.empty()) return;

    std::uniform_int_distribution<int> distrib(0, emptyCells.size() - 1);
    auto [row, col] = emptyCells[distrib(gen)];

    std::uniform_int_distribution<int> valDistrib(0, 9);
    int value = (valDistrib(gen) < 9) ? 2 : 4;

    cell[row][col] = value;
    CurScore += value;
    --FreeCells;
}

void init() {
    for (auto& row : cell)
        row.fill(0);

    BestScore = std::max(BestScore, CurScore);
    CurScore = 0;
    FreeCells = GridSize * GridSize;

    RandomCell();
}

void DrawTextCenter(float posX, float posY, const std::string& val, Color col, int fontSize, int w, int h) {
    int width = MeasureText(val.c_str(), fontSize);
    DrawText(val.c_str(), posX + w / 2 - width / 2, posY + h / 2 - fontSize / 2, fontSize, col);
}

void DrawCell(int i, int j) {
    float posX = BoardOffsetX + Step * (j + 1) + CellSize * j;
    float posY = BoardOffsetY + Step * (i + 1) + CellSize * i;
    const auto& info = cellMap[cell[i][j]];

    DrawRectangleRounded((Rectangle){posX, posY, CellSize, CellSize}, .05, 0, info.CellCol);

    if (cell[i][j] != 0)
        DrawTextCenter(posX, posY, std::to_string(cell[i][j]), info.FontCol, info.FontSz, CellSize, CellSize);
}

void DrawScore(float w = 150, float h = 70, float posXcur = 50, float posXbest = 250, float posY = 225, int fontSize = 30) {
    DrawRectangleRounded((Rectangle){posXcur, posY, w, h}, .1, 0, PlaygroundColor);
    DrawRectangleRounded((Rectangle){posXbest, posY, w, h}, .1, 0, PlaygroundColor);
    DrawTextCenter(posXcur, posY + 10, std::to_string(CurScore), WHITE, fontSize, w, h + 5);
    DrawTextCenter(posXbest, posY + 10, std::to_string(BestScore), WHITE, fontSize, w, h + 5);
    DrawTextCenter(posXcur, posY - 20, "SCORE", ColorCell2, 20, w, h + 5);
    DrawTextCenter(posXbest, posY - 20, "BEST", ColorCell2, 20, w, h + 5);
}


bool HasMovesLeft() {
    for (int i = 0; i < GridSize; ++i)
        for (int j = 0; j < GridSize; ++j)
            if (cell[i][j] == 0 ||
                (j < GridSize - 1 && cell[i][j] == cell[i][j + 1]) ||
                (i < GridSize - 1 && cell[i][j] == cell[i + 1][j]))
                return true;
    return false;
}

void Draw() {
    BeginDrawing();
    ClearBackground(BackgroundColor);

    DrawRectangleRounded((Rectangle){BoardOffsetX, BoardOffsetY, 400, 400}, .05, 0, PlaygroundColor);
    DrawTextCenter(0, 0, "2048", Color2and4, 130, ScreenWidth, 225);
    DrawTextCenter(0, 750, "Press R to restart", LIGHTGRAY, 20, ScreenWidth, 50);

    DrawScore();

    for (int i = 0; i < GridSize; ++i)
        for (int j = 0; j < GridSize; ++j)
            DrawCell(i, j);


     if (HasMovesLeft()) {
            DrawTextCenter(BoardOffsetX, BoardOffsetY, "Game Over!", RED, 50, 400, 400);
        }

    EndDrawing();
}

bool squash_column(std::array<int, GridSize>& col) {
    bool flag = false;
    int insert = GridSize - 1;
    for (int i = GridSize - 1; i >= 0; --i) {
        if (col[i] == 0) continue;
        int val = col[i];
        col[i] = 0;
        if (insert < GridSize - 1 && col[insert + 1] == val) {
            col[insert + 1] *= 2;
            CurScore += col[insert + 1];
            ++FreeCells;
            flag = true;
        } else {
            col[insert] = val;
            if (insert != i) flag = true;
            --insert;
        }
    }
    return flag;
}

void squash(int key) {
    bool moved = false;
    switch (key) {
        case KEY_DOWN:
            for (int col = 0; col < GridSize; ++col) {
                std::array<int, GridSize> tmp;
                for (int row = 0; row < GridSize; ++row)
                    tmp[row] = cell[row][col];
                moved |= squash_column(tmp);
                for (int row = 0; row < GridSize; ++row)
                    cell[row][col] = tmp[row];
            }
            break;
        case KEY_UP:
            for (int col = 0; col < GridSize; ++col) {
                std::array<int, GridSize> tmp;
                for (int row = 0; row < GridSize; ++row)
                    tmp[row] = cell[GridSize - 1 - row][col];
                moved |= squash_column(tmp);
                for (int row = 0; row < GridSize; ++row)
                    cell[GridSize - 1 - row][col] = tmp[row];
            }
            break;
        case KEY_RIGHT:
            for (int row = 0; row < GridSize; ++row) {
                std::array<int, GridSize> tmp = cell[row];
                moved |= squash_column(tmp);
                cell[row] = tmp;
            }
            break;
        case KEY_LEFT:
            for (int row = 0; row < GridSize; ++row) {
                std::array<int, GridSize> tmp;
                for (int col = 0; col < GridSize; ++col)
                    tmp[col] = cell[row][GridSize - 1 - col];
                moved |= squash_column(tmp);
                for (int col = 0; col < GridSize; ++col)
                    cell[row][GridSize - 1 - col] = tmp[col];
            }
            break;
    }

    if (moved) RandomCell();
}

void Input() {
    int key = GetKeyPressed();
    if (key == KEY_R) {
        init();
    } else {
        squash(key);
    }
}

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(ScreenWidth, ScreenHeight, "2048");

    SearchAndSetResourceDir("resources");
    Image icon = LoadImage("2048.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetWindowTitle("2048");

    LoadBestScore();
    init();

    while (!WindowShouldClose()) {
        Draw();
        Input();

    }

    SaveBestScore();
    CloseWindow();
    return 0;
}
