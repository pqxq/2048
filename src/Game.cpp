#include "Game.hpp"
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

Game::Game() 
    : m_score(0)
    , m_bestScore(0)
    , m_freeCells(Config::GridSize * Config::GridSize)
    , m_state(GameState::Playing) 
{
    std::random_device rd;
    m_rng.seed(rd());

    LoadBestScore();
    Reset();
}

void Game::Reset() {
    for (auto& row : m_grid) {
        row.fill(0);
    }

    if (m_score > m_bestScore) {
        m_bestScore = m_score;
        SaveBestScore();
    }

    m_score = 0;
    m_freeCells = Config::GridSize * Config::GridSize;
    m_state = GameState::Playing;

    SpawnRandomCell();
}

void Game::SaveBestScore() {
    std::ofstream out("./resources/bestscore.dat");
    if (out.is_open()) {
        out << m_bestScore;
    }
}

void Game::LoadBestScore() {
    std::ifstream in("./resources/bestscore.dat");
    if (in.is_open()) {
        if (!(in >> m_bestScore)) {
            m_bestScore = 0;
        }
    }
}

void Game::SpawnRandomCell() {
    if (m_freeCells == 0) return;

    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < Config::GridSize; ++i) {
        for (int j = 0; j < Config::GridSize; ++j) {
            if (m_grid[i][j] == 0) {
                emptyCells.emplace_back(i, j);
            }
        }
    }

    if (emptyCells.empty()) return;

    std::uniform_int_distribution<int> distrib(0, static_cast<int>(emptyCells.size()) - 1);
    auto [row, col] = emptyCells[distrib(m_rng)];

    std::uniform_int_distribution<int> valDistrib(0, 9);
    int value = (valDistrib(m_rng) < 9) ? 2 : 4;

    m_grid[row][col] = value;
    
    m_freeCells--;
}

bool Game::CanMove() const {
    if (m_freeCells > 0) return true;

    for (int i = 0; i < Config::GridSize; ++i) {
        for (int j = 0; j < Config::GridSize; ++j) {
            int current = m_grid[i][j];
            
            if (j < Config::GridSize - 1 && current == m_grid[i][j + 1]) return true;
            if (i < Config::GridSize - 1 && current == m_grid[i + 1][j]) return true;
        }
    }
    return false;
}

bool Game::SquashColumn(std::array<int, Config::GridSize>& col) {
    bool moved = false;
    int insertPos = Config::GridSize - 1;

    for (int i = Config::GridSize - 1; i >= 0; --i) {
        if (col[i] == 0) continue;
        
        int val = col[i];
        col[i] = 0;

        if (insertPos < Config::GridSize - 1 && col[insertPos + 1] == val) {
            col[insertPos + 1] *= 2;
            m_score += col[insertPos + 1];
            m_freeCells++;
            moved = true;
        } else {
            col[insertPos] = val;
            if (insertPos != i) moved = true;
            --insertPos;
        }
    }
    return moved;
}

bool Game::Move(Direction dir) {
    bool moved = false;

    switch (dir) {
        case Direction::Down:
            for (int col = 0; col < Config::GridSize; ++col) {
                std::array<int, Config::GridSize> tmp;
                for (int row = 0; row < Config::GridSize; ++row)
                    tmp[row] = m_grid[row][col];
                
                moved |= SquashColumn(tmp);
                
                for (int row = 0; row < Config::GridSize; ++row)
                    m_grid[row][col] = tmp[row];
            }
            break;

        case Direction::Up:
            for (int col = 0; col < Config::GridSize; ++col) {
                std::array<int, Config::GridSize> tmp;
                for (int row = 0; row < Config::GridSize; ++row)
                    tmp[row] = m_grid[Config::GridSize - 1 - row][col];
                
                moved |= SquashColumn(tmp);
                
                for (int row = 0; row < Config::GridSize; ++row)
                    m_grid[Config::GridSize - 1 - row][col] = tmp[row];
            }
            break;

        case Direction::Right:
            for (int row = 0; row < Config::GridSize; ++row) {
                std::array<int, Config::GridSize> tmp = m_grid[row];
                
                moved |= SquashColumn(tmp);
                
                m_grid[row] = tmp;
            }
            break;

        case Direction::Left:
            for (int row = 0; row < Config::GridSize; ++row) {
                std::array<int, Config::GridSize> tmp;
                for (int col = 0; col < Config::GridSize; ++col)
                    tmp[col] = m_grid[row][Config::GridSize - 1 - col];
                
                moved |= SquashColumn(tmp);
                
                for (int col = 0; col < Config::GridSize; ++col)
                    m_grid[row][Config::GridSize - 1 - col] = tmp[col];
            }
            break;
    }

    return moved;
}

void Game::Update(Direction dir) {
    if (m_state == GameState::GameOver) return;

    if (Move(dir)) {
        SpawnRandomCell();
        
        if (!CanMove()) {
            m_state = GameState::GameOver;
            if (m_score > m_bestScore) {
                m_bestScore = m_score;
                SaveBestScore();
            }
        }
    }
}