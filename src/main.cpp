#include "raylib.h"
#include "Game.hpp"
#include "Renderer.hpp"

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(Config::DesignWidth, Config::DesignHeight, "2048");

    Image icon = LoadImage("./image/logo.png"); 
    SetWindowIcon(icon);
    UnloadImage(icon);

    Game game;
    Renderer renderer;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            game.Reset();
        } else if (IsKeyPressed(KEY_UP)) {
            game.Update(Direction::Up);
        } else if (IsKeyPressed(KEY_DOWN)) {
            game.Update(Direction::Down);
        } else if (IsKeyPressed(KEY_LEFT)) {
            game.Update(Direction::Left);
        } else if (IsKeyPressed(KEY_RIGHT)) {
            game.Update(Direction::Right);
        }

        renderer.Draw(game);
    }

    CloseWindow();
    return 0;
}