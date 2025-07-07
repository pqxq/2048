#include "raylib.h"
#include "resource_dir.h"
#include <utility>
#include <string>
#include <unordered_map>
#include <random>
#include <chrono>
#include <array>

#define BackgroundColor (Color){251, 248, 240, 255}
#define PlaygroundColor (Color){187, 173, 160, 255}
#define EmptyCellColor (Color){205, 192, 180, 255}
#define Color2and4 (Color){119, 110, 101, 255}
#define Color8to2048 (Color){249, 246, 242, 255}
#define ColorCell2 (Color){238, 228, 218, 255}
#define ColorCell4 (Color){237, 224, 200, 255}
#define ColorCell8 (Color){242, 177, 121, 255}
#define ColorCell16 (Color){245, 149, 99, 255}
#define ColorCell32 (Color){246, 124, 96, 255}
#define ColorCell64 (Color){246, 94, 59, 255}
#define ColorCell128 (Color){237, 207, 115, 255}
#define ColorCell256 (Color){237, 204, 98, 255}
#define ColorCell512 (Color){237, 200, 80, 255}
#define ColorCell1024 (Color){237, 197, 63, 255}
#define ColorCell2048 (Color){237, 194, 45, 255}
#define STEP 12
#define CellSize 85

struct CellInfo
{
    Color FontCol;
    Color CellCol;
    int FontSz;
};

std::unordered_map<int, CellInfo> m{
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
    {2048, {Color8to2048, ColorCell2048, 30}}};

std::array<std::array<int, 4>, 4> cell;

int FreeCells;

std::random_device rd;
std::mt19937 gen(rd());

int CurScore = 0, BestScore = 0;

void RandomCell()
{
    if (FreeCells == 0)
    {
        return;
    }

    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (cell[i][j] == 0)
            {
                emptyCells.push_back({i, j});
            }
        }
    }

    if (emptyCells.empty())
    {
        return;
    }

    std::uniform_int_distribution<int> distrib(0, emptyCells.size() - 1);
    int randomIndex = distrib(gen);

    int row = emptyCells[randomIndex].first;
    int col = emptyCells[randomIndex].second;

    std::uniform_int_distribution<int> val_distrib(0, 9);
    if (val_distrib(gen) < 9)
    {
        cell[row][col] = 2;
        CurScore += 2;
    }
    else
    {
        cell[row][col] = 4;
        CurScore += 4;
    }

    FreeCells--;
}

void init()
{

    for(int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            cell[i][j] = 0;
        }
    }

    BestScore = std::max(BestScore, CurScore);
    CurScore = 0;
    FreeCells = 16;

    RandomCell();
}

void DrawTextCenter(float posX, float posY, std::string val, Color col, int fontSize, int w, int h)
{
    int width = MeasureText(val.c_str(), fontSize);
    DrawText(val.c_str(), posX + w / 2 - width / 2, posY + h / 2 - fontSize / 2, fontSize, col);
}

void DrawCell(int i, int j)
{
    float posX = 25 + STEP * (i + 1) + CellSize * i, posY = 350 + STEP * (j + 1) + CellSize * j;
    DrawRectangleRounded((Rectangle){posX, posY, 85, 85}, .05, 0, m[cell[i][j]].CellCol);

    if (cell[i][j] != 0)
        DrawTextCenter(posX, posY, std::to_string(cell[i][j]), m[cell[i][j]].FontCol, m[cell[i][j]].FontSz, CellSize, CellSize);
}

void DrawScore(float w = 150, float h = 70, float posXcur = 50, float posXbest = 250, float posY = 225, int fontSize = 30 ){

    DrawRectangleRounded((Rectangle){posXcur, posY, w, h}, .1, 0, PlaygroundColor);
    DrawRectangleRounded((Rectangle){posXbest, posY, w, h}, .1, 0, PlaygroundColor);
    DrawTextCenter(posXcur, posY + 10, std::to_string(CurScore), WHITE, fontSize, w, h + 5);
    DrawTextCenter(posXbest, posY + 10, std::to_string(BestScore), WHITE, fontSize, w, h + 5);
    DrawTextCenter(posXcur, posY - 20, "SCORE", ColorCell2, 20, w, h + 5);
    DrawTextCenter(posXbest, posY - 20, "BEST", ColorCell2, 20, w, h + 5);
}

void Draw()
{

    BeginDrawing();

    ClearBackground(BackgroundColor);

    DrawRectangleRounded((Rectangle){25, 350, 400, 400}, .05, 0, PlaygroundColor);

    DrawTextCenter(0, 0, "2048", Color2and4, 130, 450, 225);

    DrawTextCenter(0, 750, "Press R to restart", LIGHTGRAY, 20, 450, 50);

    DrawScore();

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            DrawCell(i, j);
        }
    }

    EndDrawing();
}

bool squash_column(std::array<int, 4> &col)
{

    bool flag = false;

    for (int dest = 3; dest >= 0; --dest)
    {
        int src = dest - 1;
        while (src >= 0 && col[src] == 0)
            --src;
        if (src < 0)
            break;
        if (col[dest] == col[src])
        {
            col[dest] = col[dest] * 2;
            col[src] = 0;
            FreeCells++;
            flag = true;
            CurScore += col[dest];
        }
        if (col[dest] == 0)
        {
            std::swap(col[dest], col[src]);
            ++dest;
            flag = true;
        }
    }
    return flag;
}

void flip_vertically()
{
    for (int x = 0; x < 4; ++x)
    {
        std::swap(cell[x][0], cell[x][3]);
        std::swap(cell[x][1], cell[x][2]);
    }
}

void flip_diagonally()
{
    std::swap(cell[1][0], cell[0][1]);
    std::swap(cell[2][0], cell[0][2]);
    std::swap(cell[3][0], cell[0][3]);
    std::swap(cell[2][1], cell[1][2]);
    std::swap(cell[3][1], cell[1][3]);
    std::swap(cell[3][2], cell[2][3]);
}

void squash(int key)
{
    bool flag = false;
    switch (key)
    {
    case KEY_DOWN:
        for (int i = 0; i < 4; ++i)
        {
            if (squash_column(cell[i]) == true)
                flag = true;
        }
        break;

    case KEY_UP:
        flip_vertically();
        for (int i = 0; i < 4; ++i)
        {
            if (squash_column(cell[i]) == true)
                flag = true;
        }
        flip_vertically();
        break;

    case KEY_RIGHT:
        flip_diagonally();
        for (int i = 0; i < 4; ++i)
        {
            if (squash_column(cell[i]) == true)
                flag = true;
        }
        flip_diagonally();
        break;

    case KEY_LEFT:
        flip_diagonally();
        flip_vertically();
        for (int i = 0; i < 4; ++i)
        {
            if (squash_column(cell[i]) == true)
                flag = true;
        }
        flip_vertically();
        flip_diagonally();
        break;
    }

    if (flag)
        RandomCell();
}

void Input()
{
    int key = GetKeyPressed();

    if(key == KEY_R){
        init();
    }else{
        squash(key);
    }
}

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

    InitWindow(450, 800, "2048");

    init();

    while (!WindowShouldClose())
    {
        Draw();
        Input();

    }

    CloseWindow();
    return 0;
}
