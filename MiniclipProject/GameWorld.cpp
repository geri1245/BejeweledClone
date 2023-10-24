#include "GameWorld.h"

Cell::Cell(int type)
    : Type(type)
{
}

void Cell::Destroy()
{
    State = State::Destroyed;
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
            column.push_back(Cell(_randomDistribution(_randomEngine)));
        }
    }
}

void GameWorld::Draw()
{
    for (int i = 0; i < _gameBoard.size(); ++i) {
        auto& currentRow = _gameBoard[i];
        for (int j = 0; j < currentRow.size(); ++j) {
            if (_gameBoard[i][j].State != Cell::State::Destroyed) {
                _screen->DrawCell(i * TileSize, j * TileSize, _gameBoard[i][j].Type, TileSize);
            }
        }
    }
}

void GameWorld::TrySwitchCells(Point lhs, Point rhs)
{
    assert(lhs.x >= 0 && lhs.x < RowCount);
    assert(lhs.y >= 0 && lhs.y < ColCount);
    assert(rhs.x >= 0 && rhs.x < RowCount);
    assert(rhs.y >= 0 && rhs.y < ColCount);

    if (DistanceSquared(lhs, rhs) == 1) {
        Cell tmp = _gameBoard[lhs.x][lhs.y];
        _gameBoard[lhs.x][lhs.y] = _gameBoard[rhs.x][rhs.y];
        _gameBoard[rhs.x][rhs.y] = tmp;

        UpdateBoardState();
    }
}

std::optional<Point> GameWorld::GetTileIndicesAtPoint(Point position)
{
    Point possibleResult = position / TileSize;
    if (possibleResult.x >= 0 && possibleResult.x < RowCount && possibleResult.y >= 0 && possibleResult.y < ColCount) {
        return possibleResult;
    }

    return std::nullopt;
}

void GameWorld::UpdateBoardState()
{
    std::vector<Point> cellsToRemove;

    for (int i = 0; i < RowCount; ++i) {
        int j = 0;
        while (j < ColCount - 2) {
            int k = j + 1;
            while (k < ColCount && _gameBoard[i][j].Type == _gameBoard[i][k].Type) {
                ++k;
            }

            // Check if we have at least 3 of the same cell types next to each other
            if (k - j > 2) {
                for (int copyInd = j; copyInd < k; ++copyInd) {
                    cellsToRemove.push_back(Point { i, copyInd });
                }
            }

            j = k;
        }
    }

    for (int j = 0; j < ColCount; ++j) {
        int i = 0;
        while (i < RowCount - 2) {
            int k = i + 1;
            while (k < RowCount && _gameBoard[i][j].Type == _gameBoard[k][j].Type) {
                ++k;
            }

            // Check if we have at least 3 of the same cell types next to each other
            if (k - i > 2) {
                for (int copyInd = i; copyInd < k; ++copyInd) {
                    cellsToRemove.push_back(Point { copyInd, j });
                }
            }

            i = k;
        }
    }

    std::sort(cellsToRemove.begin(), cellsToRemove.end(), std::greater<Point>());
    cellsToRemove.erase(std::unique(cellsToRemove.begin(), cellsToRemove.end()), cellsToRemove.end());

    for (const auto& cell : cellsToRemove) {
        _gameBoard[cell.x][cell.y].Destroy();
    }
}
