#pragma once

#include "GameWorld.h"
#include "Player.h"
#include "Screen.h"

class Game {
public:
    Game();

    void RunMainLoop();

private:
    static constexpr int FrameTime = int(1000.f / 120);

    std::unique_ptr<Screen> _screen;
    std::unique_ptr<InputProcessor> _inputProcessor;
    std::unique_ptr<GameWorld> _gameWorld;
    std::unique_ptr<Player> _player;

    bool _shouldQuit = false;

    void ProcessEvents();
};
