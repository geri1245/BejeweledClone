#pragma once

#include "Event.h"
#include "Point.h"
#include "Screen.h"

#include <optional>
#include <random>
#include <vector>

struct Cell {
    enum class State {
        Destroyed,
        Idle,
        Dragged,
        Animating
    };

    Cell(int cellTypeId);

    void Destroy();

    int Type = -1;
    State State = State::Idle;
};

class GameWorld {
public:
    using GameBoard = std::vector<std::vector<Cell>>;

    const int RowCount, ColCount;

    GameWorld(int rowCount, int colCount, int tileKindCount, Screen& screen);

    void Draw();
    void TrySwitchCells(Point lhs, Point rhs);

    std::optional<Point> GetTileIndicesAtPoint(Point position);

private:
    static constexpr int TileSize = 70; // The provided assets have this size, so for now just use it

    void UpdateBoardState();

    GameBoard _gameBoard;
    Screen* _screen;

    std::random_device _randomDevice;
    std::mt19937 _randomEngine;
    std::uniform_int_distribution<int> _randomDistribution;
};
