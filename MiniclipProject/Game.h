#pragma once

#include "GameWorld.h"
#include "MainMenu.h"
#include "Player.h"
#include "Screen.h"

class Game {
public:
    Game();

    void RunMainLoop();

private:
    enum class GameState {
        Paused,
        Playing,
    };

    static constexpr int DesiredFPS = 60;
    static constexpr int FrameTime = int(1000.f / DesiredFPS);

    std::unique_ptr<Screen> _screen;
    std::unique_ptr<InputProcessor> _inputProcessor;
    std::unique_ptr<GameWorld> _gameWorld;
    std::unique_ptr<MainMenu> _menu;
    std::unique_ptr<Player> _player;

    bool _shouldQuit = false;
    GameState _gameState = GameState::Paused;

    std::unique_ptr<EventToken> _keyPressedToken;
    std::unique_ptr<EventToken> _mouseClickedToken;

    void ProcessEvents();
    void HandleKeyPress(Key key);
    void HandleButtonClicked(ButtonType button);
    void ToggleIsPlaying();
};
