#pragma once

#include "Event.h"
#include "InputProcessor.h"
#include "Screen.h"

enum class ButtonType {
    Resume,
    BestMove,
    Quit,
    Classic,
    QuickDeath,
};

class MainMenu {
public:
    MainMenu(const Screen& screen, InputProcessor& inputProcessor);

    void Draw();
    void Activate(bool needsResumeButton);
    void Deactivate();

    Event<std::function<void(ButtonType clickedButton)>> ButtonClicked;

private:
    static constexpr int ButtonHeight = 40;
    static constexpr int ButtonWidth = 280;
    static constexpr int ButtonSpacing = 20;

    struct Button {
        ButtonType Type;
        std::string Text;
        SDL_Rect Position;
    };

    const Screen* _screen;
    InputProcessor* _inputProcessor;
    std::unique_ptr<EventToken> _userClickedEventToken;
    std::vector<Button> _buttons;

    std::vector<ButtonType> _buttonTypes;

    std::unique_ptr<EventToken> _mouseClickedEventToken;
    std::unique_ptr<EventToken> _mouseMovedEventToken;
    std::optional<ButtonType> _hoveredButton;

    void MakeMenuFromButtonTypes();
    void TryClick(Vec2 position);
    void TryHover(Vec2 position);
};
