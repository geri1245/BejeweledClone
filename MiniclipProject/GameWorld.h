#pragma once

#include "Event.h"
#include "Screen.h"
#include "Vec2.h"

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

    Event<std::function<void(Vec2 lhs, Vec2 rhs)>> TileDragCompleted;

    GameWorld(int rowCount, int colCount, int tileKindCount, Screen& screen);

    void Draw();
    void Update(uint64_t now);
    bool IsInteractionEnabled() const;

    void SetActiveCell(std::optional<Vec2> index, Vec2 offset = Vec2 { 0, 0 });

    bool TrySwitchCells(Vec2 source, Vec2 destination, bool isDraggedCellTheSource = false);

    std::optional<Vec2> GetTileIndicesAtPoint(Vec2 position);

private:
    struct CellMoveData {
        Vec2 StartingPosition;
        Vec2 FinalPosition;
        int CellType;
        std::optional<Vec2> StartPositionOverride;
    };

    struct CellDestructionData {
        Vec2 CellIndex;
        int CellType;
    };

    struct AnimationState {
        std::variant<std::vector<CellMoveData>, std::vector<CellDestructionData>> AnimationData;
        std::function<void()> Completion;
        uint64_t AnimationStartTime = 0;
        double AnimationDuration = 0;
        double AnimationProgress = 0.0;
        Cell::CellState FinalCellState = Cell::CellState::Normal;
    };

    struct ActiveCellState {
        Vec2 Index;
        Vec2 Offset;
        uint64_t AnimationStartTime = 0;
    };

    static constexpr int TileSize = 70; // The provided assets have this size, so for now just use it
    static constexpr int DragOffsetSuccessThreshold = int(TileSize * 0.8);
    static constexpr double CellSwitchAnimationDurationMs = 200.0;
    static constexpr double CellDestroyAnimationDurationMs = 1000.0;
    static constexpr double CellFallAnimationDurationMs = 800.0;

    Cell& At(Vec2 indices);
    const Cell& At(Vec2 indices) const;

    Cell GetRandomCell();
    void UpdateBoardState();
    void MoveCellsAnimated(std::vector<CellMoveData>&& moveData, double animationTime, std::function<void()> completion);
    void DestroyCellsAnimated(std::vector<Vec2>&& cellsToDestroy, double animationTime, std::function<void()> completion);
    void MoveDownCells();

    bool IsIndexOnTheBoard(Vec2 index) const;

    // The board is stored in a column major order. Columns are growing from left to right. Rows are growing from top to bottom.
    GameBoard _gameBoard;
    Screen* _screen = nullptr;

    std::random_device _randomDevice;
    std::mt19937 _randomEngine;
    std::uniform_int_distribution<int> _randomDistribution;
    std::optional<AnimationState> _animationState;
    std::optional<ActiveCellState> _activeCellState;
};
