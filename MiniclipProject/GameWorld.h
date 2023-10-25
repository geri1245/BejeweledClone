#pragma once

#include "Event.h"
#include "Point.h"
#include "Screen.h"

#include <optional>
#include <random>
#include <variant>
#include <vector>

struct Cell {
    enum class CellState {
        Destroyed,
        Normal,
        Active,
        WaitingForAnimationToComplete,
    };

    Cell(int cellTypeId);

    void Destroy();

    int Type = -1;
    CellState State = CellState::Normal;
};

class GameWorld {
public:
    using GameBoard = std::vector<std::vector<Cell>>;

    const int RowCount, ColCount;

    GameWorld(int rowCount, int colCount, int tileKindCount, Screen& screen);

    void Draw();
    void Update(uint64_t now);
    bool IsInteractionEnabled() const;

    void SetActiveCell(std::optional<Point> index, Point offset = Point { 0, 0 });

    void TrySwitchCells(Point lhs, Point rhs);

    std::optional<Point> GetTileIndicesAtPoint(Point position);

private:
    struct CellMoveData {
        Point StartingPosition;
        Point FinalPosition;
        int CellType;
    };

    struct AnimationState {
        std::vector<CellMoveData> AnimationData;
        std::function<void()> Completion;
        uint64_t AnimationStartTime = 0;
        double AnimationDuration = 0;
        double AnimationProgress = 0.0;
        Cell::CellState FinalCellState = Cell::CellState::Normal;
    };

    struct ActiveCellState {
        Point Index;
        Point Offset;
        uint64_t AnimationStartTime = 0;
    };

    static constexpr int TileSize = 70; // The provided assets have this size, so for now just use it
    static constexpr double CellSwitchAnimationDurationMs = 200.0;
    static constexpr double CellDestroyAnimationDurationMs = 1000.0;
    static constexpr double CellFallAnimationDurationMs = 800.0;

    Cell& At(Point indices);
    const Cell& At(Point indices) const;

    Cell GetRandomCell();
    void UpdateBoardState();
    void MoveCellsAnimated(std::vector<CellMoveData>&& moveData, double animationTime, std::function<void()> completion);
    void DestroyCellsAnimated(std::vector<Point>&& cellsToDestroy, double animationTime, std::function<void()> completion);
    void MoveDownCells();

    GameBoard _gameBoard;
    Screen* _screen;

    std::random_device _randomDevice;
    std::mt19937 _randomEngine;
    std::uniform_int_distribution<int> _randomDistribution;
    std::optional<AnimationState> _animationState;
    std::optional<ActiveCellState> _activeCellState;
};
