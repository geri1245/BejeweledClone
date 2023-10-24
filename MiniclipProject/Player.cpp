#include "Player.h"

Player::Player(InputProcessor& inputProcessor, GameWorld& gameWorld)
    : _gameWorld(&gameWorld)
    , _mouseMoveToken(inputProcessor.MouseClicked.Subscribe([this](Point position) { OnMouseClicked(position); }))
{
}

void Player::OnMouseClicked(Point point)
{
    _draggedCell.reset();

    if (_selectedCell) {
        auto newSelectedCell = _gameWorld->GetTileIndicesAtPoint(point);
        if (newSelectedCell) {
            _gameWorld->TrySwitchCells(*_selectedCell, *newSelectedCell);
        }

        _selectedCell.reset();
    } else {
        _selectedCell = _gameWorld->GetTileIndicesAtPoint(point);
    }
}

void Player::OnMouseDragStarted(Point position)
{
    _draggedCell = _gameWorld->GetTileIndicesAtPoint(position); // This will take care of the "out-of-board" case as well
}

void Player::OnMouseDragMoved(Point position)
{
    if (_draggedCell) {
        if (auto newCell = _gameWorld->GetTileIndicesAtPoint(position)) {
        }
    }
}

void Player::OnMouseDragEnded(Point position)
{
    if (_draggedCell) {
        _draggedCell.reset();
    }
}
