#pragma once
#include "Constants.hpp"
#include <array>
#include <random>

enum class Direction { Up, Down, Left, Right };
enum class GameState { Playing, GameOver };

class Game {
public:
    using Grid = std::array<std::array<int, Config::GridSize>, Config::GridSize>;

    Game();
    void Reset();
    void Update(Direction dir);
    
    const Grid& GetGrid() const { return m_grid; }
    int GetScore() const { return m_score; }
    int GetBestScore() const { return m_bestScore; }
    bool IsGameOver() const { return m_state == GameState::GameOver; }

private:
    void SpawnRandomCell();
    bool CanMove() const;
    bool Move(Direction dir);
    bool SquashColumn(std::array<int, Config::GridSize>& col);
    
    void SaveBestScore();
    void LoadBestScore();

    Grid m_grid;
    int m_score;
    int m_bestScore;
    int m_freeCells;
    GameState m_state;
    
    std::mt19937 m_rng;
};