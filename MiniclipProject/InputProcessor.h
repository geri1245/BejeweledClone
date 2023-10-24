#pragma once

#include "Event.h"
#include "Point.h"

#include <SDL.h>

#include <optional>

enum class Key {
    Escape,
};

class InputProcessor {
public:
    void ProcessKeyEvent(const SDL_Event& keyEvent);
    void ProcessMouseEvent(const SDL_Event& mouseEvent);

    Event<std::function<void(Point position)>> MouseDragStarted;
    Event<std::function<void(Point position)>> MouseDragMoved;
    Event<std::function<void(Point position)>> MouseDragEnded;
    Event<std::function<void(Point position)>> MouseClicked;

    Event<std::function<void(Key key)>> KeyPressed;

private:
    bool _isDragging = false;
    std::optional<Point> _mouseDragStartPosition;
};
