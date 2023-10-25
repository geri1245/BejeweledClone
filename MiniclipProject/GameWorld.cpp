#include "GameWorld.h"

Cell::Cell(int type)
    : Type(type)
{
}

void Cell::Destroy()
{
    State = CellState::Destroyed;
    Type = -1;
}

GameWorld::GameWorld(int rowCount, int colCount, int tileKindCount, Screen& screen)
    : RowCount(rowCount)
    , ColCount(colCount)
    , _screen(&screen)
    , _randomEngine(_randomDevice())
    , _randomDistribution(0, tileKindCount - 1) // Random distribution is inclusive on both ends, so the range [0, n - 1] will contain n possible values
{
    _gameBoard.reserve(colCount);

    for (int i = 0; i < rowCount; ++i) {
        auto& column = _gameBoard.emplace_back();
        column.reserve(rowCount);
        for (int j = 0; j < rowCount; ++j) {
            column.push_back(GetRandomCell());
        }
    }
}

void GameWorld::Draw()
{
    for (int i = 0; i < _gameBoard.size(); ++i) {
        auto& currentRow = _gameBoard[i];
        for (int j = 0; j < currentRow.size(); ++j) {
            if (_gameBoard[i][j].State == Cell::CellState::Normal) {
                _screen->DrawCell(Vec2 { i * TileSize, j * TileSize }, _gameBoard[i][j].Type, TileSize);
            }
        }
    }

    if (_activeCellState) {
        auto now = SDL_GetTicks64();

        // Make a periodic function with a period of 1 second and in the range [0, 0.2]
        auto scaleDiff = (sin((now - _activeCellState->AnimationStartTime) / 1000.f * 2 * M_PI) + 1) * 0.1;
        auto newSize = TileSize * (1 + scaleDiff);
        auto halfDiff = int((newSize - TileSize) / 2.0);

        _screen->DrawCell(
            _activeCellState->Index * TileSize - Vec2 { halfDiff, halfDiff } + _activeCellState->Offset,
            At(_activeCellState->Index).Type,
            int(newSize));
    }

    if (_animationState) {
        for (const auto& [startPosition, endPosition, cellType, startPositionOverride] : _animationState->AnimationData) {
            auto realStartPosition = startPositionOverride.value_or(startPosition);
            _screen->DrawCell(realStartPosition.Lerp(endPosition, _animationState->AnimationProgress), cellType, TileSize);
        }
    }
}

void GameWorld::Update(uint64_t now)
{
    if (_animationState) {
        double rawProgress = (now - _animationState->AnimationStartTime) / _animationState->AnimationDuration;

        if (rawProgress > 1.0) {
            auto completion = std::move(_animationState->Completion);

            for (auto& column : _gameBoard) {
                for (auto& cell : column) {
                    if (cell.State == Cell::CellState::WaitingForAnimationToComplete) {
                        cell.State = _animationState->FinalCellState;
                    }
                }
            }

            _animationState.reset();

            if (completion) {
                completion();
            }
        } else {
            _animationState->AnimationProgress = pow(rawProgress, 3);
        }
    }
}

bool GameWorld::IsInteractionEnabled() const
{
    return !_animationState.has_value();
}

void GameWorld::SetActiveCell(std::optional<Vec2> index, Vec2 offset)
{
    if (index) {
        if (abs(offset.x) > DragOffsetSuccessThreshold) { // Successful drag in the x direction
            if (auto newCell = *index + Vec2 { offset.x > 0 ? 1 : -1, 0 }; IsIndexOnTheBoard(newCell)) {
                if (TrySwitchCells(*index, newCell, true)) {
                    TileDragCompleted.Invoke(*index, newCell);
                }
            }
        } else if (abs(offset.y) > DragOffsetSuccessThreshold) { // Successful drag in the y direction
            if (auto newCell = *index + Vec2 { 0, offset.y > 0 ? 1 : -1 }; IsIndexOnTheBoard(newCell)) {
                if (TrySwitchCells(*index, newCell, true)) {
                    TileDragCompleted.Invoke(*index, newCell);
                }
            }
        } else { // Just update the drag state (eg. cell position)
            if (_activeCellState && _activeCellState->Index == index) {
                _activeCellState->Offset = offset;
            } else {
                _activeCellState.emplace(
                    *index, offset, SDL_GetTicks64());
                At(*index).State = Cell::CellState::Active;
            }
        }
    } else {
        if (_activeCellState) {
            auto activeIndex = _activeCellState->Index;
            if (At(activeIndex).State == Cell::CellState::Active) {
                At(activeIndex).State = Cell::CellState::Normal;

                if (offset != Vec2 { 0, 0 }) {
                    MoveCellsAnimated({ CellMoveData { Vec2 {},
                                          activeIndex,
                                          At(activeIndex).Type,
                                          activeIndex * TileSize + _activeCellState->Offset } },
                        CellSwitchAnimationDurationMs, nullptr);
                }
            }
            _activeCellState.reset();
        }
    }
}

bool GameWorld::TrySwitchCells(Vec2 lhs, Vec2 rhs, bool isDraggedCellTheSource)
{
    assert(lhs.x >= 0 && lhs.x < RowCount);
    assert(lhs.y >= 0 && lhs.y < ColCount);
    assert(rhs.x >= 0 && rhs.x < RowCount);
    assert(rhs.y >= 0 && rhs.y < ColCount);
    assert(!isDraggedCellTheSource || _activeCellState);

    if (lhs.DistanceSquared(rhs) == 1) {
        MoveCellsAnimated(
            {
                CellMoveData { lhs, rhs, At(lhs).Type, isDraggedCellTheSource ? _activeCellState->Index * TileSize + _activeCellState->Offset : std::optional<Vec2>() },
                CellMoveData { rhs, lhs, At(rhs).Type, std::nullopt },
            },
            CellSwitchAnimationDurationMs, [this]() { UpdateBoardState(); });

        return true;
    }

    return false;
}

std::optional<Vec2> GameWorld::GetTileIndicesAtPoint(Vec2 position)
{
    Vec2 possibleResult = position / TileSize;
    if (possibleResult.x >= 0 && possibleResult.x < RowCount && possibleResult.y >= 0 && possibleResult.y < ColCount) {
        return possibleResult;
    }

    return std::nullopt;
}

Cell& GameWorld::At(Vec2 indices)
{
    assert(indices.x >= 0 && indices.x < ColCount);
    assert(indices.y >= 0 && indices.y < RowCount);

    return _gameBoard[indices.x][indices.y];
}

const Cell& GameWorld::At(Vec2 indices) const
{
    assert(indices.x >= 0 && indices.x < ColCount);
    assert(indices.y >= 0 && indices.y < RowCount);

    return _gameBoard[indices.x][indices.y];
}

Cell GameWorld::GetRandomCell()
{
    return _randomDistribution(_randomEngine);
}

void GameWorld::UpdateBoardState()
{
    std::vector<Vec2> cellsToRemove;

    // Check the rows for at least 3 of the same cells next to each other
    for (int i = 0; i < RowCount; ++i) {
        int j = 0;
        while (j < ColCount - 2) {
            int k = j + 1;

            // Keep going until we find a cell that is different from the current one
            while (k < ColCount && _gameBoard[i][j].Type == _gameBoard[i][k].Type) {
                ++k;
            }

            // Check if we have at least 3 of the same cell types next to each other
            if (k - j > 2) {
                for (int copyInd = j; copyInd < k; ++copyInd) {
                    cellsToRemove.push_back(Vec2 { i, copyInd });
                }
            }

            j = k;
        }
    }

    // Exact same logic for the columns as well
    for (int j = 0; j < ColCount; ++j) {
        int i = 0;
        while (i < RowCount - 2) {
            int k = i + 1;

            while (k < RowCount && _gameBoard[i][j].Type == _gameBoard[k][j].Type) {
                ++k;
            }

            if (k - i > 2) {
                for (int copyInd = i; copyInd < k; ++copyInd) {
                    cellsToRemove.push_back(Vec2 { copyInd, j });
                }
            }

            i = k;
        }
    }

    if (!cellsToRemove.empty()) {
        std::sort(cellsToRemove.begin(), cellsToRemove.end(), std::greater<Vec2>());
        cellsToRemove.erase(std::unique(cellsToRemove.begin(), cellsToRemove.end()), cellsToRemove.end());

        for (auto& cell : cellsToRemove) {
            At(cell).Destroy();
        }

        DestroyCellsAnimated(std::move(cellsToRemove), CellDestroyAnimationDurationMs, [this]() { MoveDownCells(); });
    }
}

void GameWorld::MoveCellsAnimated(std::vector<CellMoveData>&& moveData, double animationDuration, std::function<void()> completion)
{
    assert(!_animationState);
    _animationState.emplace();

    _animationState->AnimationData = std::move(moveData);

    for (auto& animationData : _animationState->AnimationData) {
        auto& finalCell = At(animationData.FinalPosition);
        finalCell.State = Cell::CellState::WaitingForAnimationToComplete;
        finalCell.Type = animationData.CellType;

        animationData.FinalPosition = animationData.FinalPosition * TileSize;
        animationData.StartingPosition = animationData.StartPositionOverride.value_or(animationData.StartingPosition) * TileSize;
    }

    _animationState->AnimationStartTime = SDL_GetTicks64();
    _animationState->AnimationProgress = 0.0;
    _animationState->AnimationDuration = animationDuration;
    _animationState->Completion = std::move(completion);
    _animationState->FinalCellState = Cell::CellState::Normal;
}

void GameWorld::DestroyCellsAnimated(std::vector<Vec2>&& cellsToDestroy, double animationTime, std::function<void()> completion)
{
    _animationState.emplace();

    _animationState->AnimationStartTime = SDL_GetTicks64();
    _animationState->AnimationProgress = 0.0;
    _animationState->AnimationDuration = CellDestroyAnimationDurationMs;
    _animationState->Completion = std::move(completion);
    _animationState->FinalCellState = Cell::CellState::Destroyed;
}

void GameWorld::MoveDownCells()
{
    std::vector<CellMoveData> cellMoveData;

    // Update the position of every cell that is above a destroyed cell and add them to be animated
    for (int i = 0; i < ColCount; ++i) {
        int destroyedCellCount = 0;

        for (int j = RowCount - 1; j >= 0; --j) {
            if (_gameBoard[i][j].State == Cell::CellState::Destroyed) {
                ++destroyedCellCount;
            } else if (destroyedCellCount > 0) {
                int newRow = j + destroyedCellCount;

                auto startPosition = Vec2 { i, j };
                auto finalPosition = Vec2 { i, newRow };

                cellMoveData.push_back(CellMoveData { startPosition, finalPosition, At(startPosition).Type });

                At(startPosition).State = Cell::CellState::Destroyed;
            }
        }

        // Fill the board again by generating destroyedCellCount number of new cells and animate them in from the top
        for (int cellInd = 0; cellInd < destroyedCellCount; ++cellInd) {
            auto finalPosition = Vec2 { i, cellInd };
            auto startPosition = Vec2 { i, cellInd - destroyedCellCount };
            auto newCellType = GetRandomCell().Type;

            cellMoveData.push_back(CellMoveData { startPosition, finalPosition, newCellType });
        }
    }

    MoveCellsAnimated(std::move(cellMoveData), CellFallAnimationDurationMs, [this]() { UpdateBoardState(); });
}

bool GameWorld::IsIndexOnTheBoard(Vec2 index) const
{
    return !(index.x < 0 || index.x > ColCount - 1 || index.y < 0 || index.y > RowCount - 1);
}
