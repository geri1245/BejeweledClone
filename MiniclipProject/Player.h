#pragma once

#include "GameWorld.h"
#include "InputProcessor.h"

class Player {
public:
    Player(InputProcessor& inputProcessor, GameWorld& gameWorld);

private:
    GameWorld* _gameWorld = nullptr;
    std::unique_ptr<EventToken> _mouseMoveToken;
    std::optional<Point> _draggedCell; // Used for mouse drag
    std::optional<Point> _selectedCell; // Used for mouse select (click)

    void OnMouseDragStarted(Point position);
    void OnMouseDragMoved(Point position);
    void OnMouseDragEnded(Point position);
    void OnMouseClicked(Point position);
};
