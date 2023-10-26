#include "Game.h"
#include "Event.h"
#include "InputProcessor.h"

#include <iostream>

Game::Game()
    : _screen(Screen::GetScreen())
    , _inputProcessor(std::make_unique<InputProcessor>())
{
    if (!_screen) {
        std::cerr << "Failed to initialize screen. Terminating..." << std::endl;
        std::terminate();
    }

    _gameWorld = std::make_unique<GameWorld>(8, 8, 5, *_screen);
    _player = std::make_unique<Player>(*_inputProcessor, *_gameWorld);
}

void Game::RunMainLoop()
{
    auto previous = SDL_GetTicks64();

    while (!_shouldQuit) {
        auto now = SDL_GetTicks64();

        // Don't limit the user input processing frequency
        ProcessEvents();

        _gameWorld->Update();

        // Limit the display rate at 120 Hz
        if (now - previous >= FrameTime) {
            _screen->BeginFrame();

            _gameWorld->Draw();

            _screen->Present();

            previous = SDL_GetTicks64();
        }
    }
}

void Game::ProcessEvents()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
        case SDL_QUIT: {
            _shouldQuit = true;
        } break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            _inputProcessor->ProcessMouseEvent(e);
        } break;
        case SDL_KEYDOWN: {
            _inputProcessor->ProcessKeyEvent(e);
        } break;
        default:
            break;
        }
    }
}
