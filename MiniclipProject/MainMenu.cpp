#include "MainMenu.h"

namespace {
bool Contains(const SDL_Rect& rect, Vec2 position)
{
    return position.x >= rect.x && position.y >= rect.y && position.x <= rect.x + rect.w && position.y <= rect.y + rect.h;
}
}

namespace {
std::string GetTextForButtonType(ButtonType buttonType)
{
    switch (buttonType) {
    case ButtonType::Resume:
        return "Resume Game";
        break;
    case ButtonType::BestMove:
        return "Play Best Move";
        break;
    case ButtonType::Quit:
        return "Quit Game";
        break;
    case ButtonType::Classic:
        return "Play Classic";
        break;
    case ButtonType::QuickDeath:
        return "Play Quick Death";
        break;
    }

    return "ERROR - STRING NOT FOUND!!!";
}
}

MainMenu::MainMenu(const Screen& screen, InputProcessor& inputProcessor)
    : _screen(&screen)
    , _inputProcessor(&inputProcessor)
    , _buttonTypes {
        ButtonType::Classic,
        ButtonType::QuickDeath,
        ButtonType::BestMove,
        ButtonType::Quit,
    }
{
    MakeMenuFromButtonTypes();
}

void MainMenu::Draw()
{
    for (const auto& button : _buttons) {
        _screen->DrawButton(button.Text, button.Position);
    }
}

void MainMenu::Activate(bool needsResumeButton)
{
    if (needsResumeButton && _buttonTypes[0] != ButtonType::Resume) {
        _buttonTypes.insert(_buttonTypes.begin(), ButtonType::Resume);
        MakeMenuFromButtonTypes();
    } else if (!needsResumeButton && _buttonTypes[0] == ButtonType::Resume) {
        _buttonTypes.erase(_buttonTypes.begin());
        MakeMenuFromButtonTypes();
    }

    _mouseClickedEventToken = _inputProcessor->MouseClicked.Subscribe([this](Vec2 position) { TryClick(position); });
}

void MainMenu::Deactivate()
{
    _mouseClickedEventToken.reset();
}

void MainMenu::MakeMenuFromButtonTypes()
{
    auto screenWidth = _screen->ScreenWidth;
    auto screenHeight = _screen->ScreenHeight;

    auto numOfButtons = int(_buttonTypes.size());

    auto menuSize = numOfButtons * ButtonHeight + (numOfButtons - 1) * ButtonSpacing;
    auto buttonYPosition = (screenHeight - menuSize) / 2;
    auto buttonXPosition = (screenWidth - ButtonWidth) / 2;

    _buttons.reserve(numOfButtons);
    for (auto buttonType : _buttonTypes) {
        _buttons.push_back(Button {
            buttonType,
            GetTextForButtonType(buttonType),
            SDL_Rect { buttonXPosition, buttonYPosition, ButtonWidth, ButtonHeight } });

        buttonYPosition += (ButtonHeight + ButtonSpacing);
    }
}

void MainMenu::TryClick(Vec2 position)
{
    for (const auto& button : _buttons) {
        if (Contains(button.Position, position)) {
            ButtonClicked.Invoke(button.Type);
        }
    }
}
