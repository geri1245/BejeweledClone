#pragma once

#include "GameWorld.h"
#include "InputProcessor.h"

class Player {
public:
    Player(InputProcessor& inputProcessor, GameWorld& gameWorld);

private:
    struct SelectedCell {
        SelectedCell(Point initialCoordinates, Point selectedIndex, GameWorld& gameWorld, bool isDragging);
        ~SelectedCell();

        void UpdateBoardState(Point currentPosition);
        bool IsDragging() const;
        Point Index() const;

    private:
        Point _initialCoordinates;
        Point _selectedIndex;
        GameWorld& _gameWorld;
        bool _isDragging;
    };

    GameWorld* _gameWorld = nullptr;
    std::unique_ptr<EventToken> _mouseMoveToken;
    std::unique_ptr<EventToken> _mouseDragStartedToken;
    std::unique_ptr<EventToken> _mouseDragMovedToken;
    std::unique_ptr<EventToken> _mouseDragEndedToken;
    std::optional<Point> _draggedCell; // Used for mouse drag
    std::optional<SelectedCell> _selectedCell; // Used for mouse selection

    void OnMouseDragStarted(Point position);
    void OnMouseDragMoved(Point position);
    void OnMouseDragEnded(Point position);
    void OnMouseClicked(Point position);
};
