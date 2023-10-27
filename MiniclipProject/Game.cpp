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
    _menu = std::make_unique<MainMenu>(*_screen, *_inputProcessor);
    _player = std::make_unique<Player>(*_inputProcessor, *_gameWorld);

    _keyPressedToken = _inputProcessor->KeyPressed.Subscribe([this](Key key) { HandleKeyPress(key); });
    _mouseClickedToken = _menu->ButtonClicked.Subscribe([this](ButtonType button) { HandleButtonClicked(button); });

    _menu->Activate(false);
}

void Game::RunMainLoop()
{
    auto previous = SDL_GetTicks64();

    while (!_shouldQuit) {
        auto now = SDL_GetTicks64();
        auto delta = now - previous;

        ProcessEvents();

        _screen->BeginFrame();

        switch (_gameState) {
        case Game::GameState::Paused: {
            _menu->Draw();
        } break;
        case Game::GameState::Playing: {
            _gameWorld->Update(delta);

            _gameWorld->Draw();
        } break;
        }

        _screen->Present();

        if (delta <= FrameTime) {
            SDL_Delay(uint32_t(FrameTime - delta));
        }

        previous = now;
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

void Game::HandleKeyPress(Key key)
{
    switch (key) {
    case Key::Escape: {
        ToggleIsPlaying();
    } break;
    }
}

void Game::HandleButtonClicked(ButtonType button)
{
    switch (button) {
    case ButtonType::Resume:
        ToggleIsPlaying();
        break;
    case ButtonType::BestMove:
        break;
    case ButtonType::Quit:
        break;
    case ButtonType::Classic:
        ToggleIsPlaying();
        break;
    case ButtonType::QuickDeath:
        break;
    }
}

void Game::ToggleIsPlaying()
{
    _gameState = (_gameState == GameState::Paused) ? GameState::Playing : GameState::Paused;

    switch (_gameState) {
    case Game::GameState::Paused: {
        _gameWorld->Deactivate();
        _menu->Activate(true);
    } break;
    case Game::GameState::Playing: {
        _gameWorld->Activate();
        _menu->Deactivate();
    } break;
    }
}
