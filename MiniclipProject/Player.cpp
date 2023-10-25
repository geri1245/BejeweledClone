#include "Player.h"

Player::Player(InputProcessor& inputProcessor, GameWorld& gameWorld)
    : _gameWorld(&gameWorld)
    , _mouseMoveToken(inputProcessor.MouseClicked.Subscribe([this](Point position) {
        if (_gameWorld->IsInteractionEnabled()) {
            OnMouseClicked(position);
        }
    }))
    , _mouseDragStartedToken(inputProcessor.MouseDragStarted.Subscribe([this](Point position) {
        if (_gameWorld->IsInteractionEnabled()) {
            OnMouseDragStarted(position);
        }
    }))
    , _mouseDragMovedToken(inputProcessor.MouseDragMoved.Subscribe([this](Point position) {
        if (_gameWorld->IsInteractionEnabled()) {
            OnMouseDragMoved(position);
        }
    }))
    , _mouseDragEndedToken(inputProcessor.MouseDragEnded.Subscribe([this](Point position) {
        if (_gameWorld->IsInteractionEnabled()) {
            OnMouseDragEnded(position);
        }
    }))
{
}

void Player::OnMouseClicked(Point clickedCoordinates)
{
    _draggedCell.reset();

    if (_selectedCell) {
        auto newSelectedCell = _gameWorld->GetTileIndicesAtPoint(clickedCoordinates);
        auto cell = _selectedCell->Index();

        _selectedCell.reset();

        if (newSelectedCell) {
            _gameWorld->TrySwitchCells(cell, *newSelectedCell);
        }

    } else if (auto newSelectedCell = _gameWorld->GetTileIndicesAtPoint(clickedCoordinates)) {
        _selectedCell.emplace(clickedCoordinates, *newSelectedCell, *_gameWorld, false);
    }
}

void Player::OnMouseDragStarted(Point clickedCoordinates)
{
    auto draggedCell = _gameWorld->GetTileIndicesAtPoint(clickedCoordinates);
    if (draggedCell) {
        _selectedCell.emplace(clickedCoordinates, *draggedCell, *_gameWorld, true);
    }
}

void Player::OnMouseDragMoved(Point position)
{
    if (_selectedCell && _selectedCell->IsDragging()) {
        _selectedCell->UpdateBoardState(position);
    }
}

void Player::OnMouseDragEnded(Point position)
{
    if (_selectedCell && _selectedCell->IsDragging()) {
        _selectedCell.reset();
    }
}

Player::SelectedCell::SelectedCell(Point initialCoordinates, Point selectedIndex, GameWorld& gameWorld, bool isDragging)
    : _initialCoordinates(initialCoordinates)
    , _selectedIndex(selectedIndex)
    , _gameWorld(gameWorld)
    , _isDragging(isDragging)
{
    _gameWorld.SetActiveCell(_selectedIndex);
}

Player::SelectedCell::~SelectedCell()
{
    _gameWorld.SetActiveCell(std::nullopt);
}

void Player::SelectedCell::UpdateBoardState(Point currentPosition)
{
    _gameWorld.SetActiveCell(_selectedIndex, currentPosition - _initialCoordinates);
}

bool Player::SelectedCell::IsDragging() const
{
    return _isDragging;
}

Point Player::SelectedCell::Index() const
{
    return _selectedIndex;
}
