#pragma once
#include "raylib.h"

namespace Config {
    constexpr int DesignWidth = 450; 
    constexpr int DesignHeight = 800;

    constexpr int GridSize = 4;
    constexpr int CellSize = 85;
    constexpr int Step = 12;
    constexpr int BoardOffsetX = 25;
    constexpr int BoardOffsetY = 350;

    constexpr Color Hex(unsigned char r, unsigned char g, unsigned char b) {
        return {r, g, b, 255};
    }

    namespace Colors {
        constexpr Color Background = Hex(251, 248, 240);
        constexpr Color Board      = Hex(187, 173, 160);
        constexpr Color EmptyCell  = Hex(205, 192, 180);
        constexpr Color TextDark   = Hex(119, 110, 101);
        constexpr Color TextLight  = Hex(249, 246, 242);
    }
}