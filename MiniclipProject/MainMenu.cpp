#include "MainMenu.h"

namespace {
bool Contains(const SDL_Rect& rect, Vec2 position)
{
    return position.x >= rect.x && position.y >= rect.y && position.x <= rect.x + rect.w && position.y <= rect.y + rect.h;
}

int GetCenteredPositionOfElement(int screenWidth, int elementWidth)
{
    return (screenWidth - elementWidth) / 2;
}
}

namespace {
std::string GetTextForButtonType(ButtonType buttonType)
{
    switch (buttonType) {
    case ButtonType::Resume:
        return "Resume Game";
    case ButtonType::Quit:
        return "Quit Game";
    case ButtonType::Classic:
        return "Play Classic";
    case ButtonType::QuickDeath:
        return "Play Quick Death";
    case ButtonType::Leaderboard:
        return "Show Leaderboard";
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
        ButtonType::Leaderboard,
        ButtonType::Quit,
    }
{
    MakeMenuFromButtonTypes();
}

void MainMenu::Draw()
{
    for (const auto& button : _buttons) {
        _screen->DrawButton(button.Text, button.Position, button.Type == _hoveredButton);
    }

    for (const auto& textBlock : _additionalText) {
        _screen->DrawText(textBlock.Text, textBlock.Position, true);
    }
}

void MainMenu::Activate(bool needsResumeButton, const std::vector<std::string>& additionalText)
{
    MakeTextBlocksFromTexts(additionalText);

    if (needsResumeButton && _buttonTypes[0] != ButtonType::Resume) {
        _buttonTypes.insert(_buttonTypes.begin(), ButtonType::Resume);
        MakeMenuFromButtonTypes();
    } else if (!needsResumeButton && _buttonTypes[0] == ButtonType::Resume) {
        _buttonTypes.erase(_buttonTypes.begin());
        MakeMenuFromButtonTypes();
    }

    _mouseClickedEventToken = _inputProcessor->MouseClicked.Subscribe([this](Vec2 position) { TryClick(position); });
    _mouseMovedEventToken = _inputProcessor->MouseMoved.Subscribe([this](Vec2 position) { TryHover(position); });
}

void MainMenu::Deactivate()
{
    _mouseClickedEventToken.reset();
    _mouseMovedEventToken.reset();

    _hoveredButton.reset();
}

void MainMenu::MakeMenuFromButtonTypes()
{
    auto screenWidth = _screen->ScreenWidth;
    auto screenHeight = _screen->ScreenHeight;

    auto numOfButtons = int(_buttonTypes.size());

    auto menuSize = numOfButtons * ButtonHeight + (numOfButtons - 1) * ButtonSpacing;
    auto buttonYPosition = (screenHeight - menuSize) / 2;
    auto buttonXPosition = GetCenteredPositionOfElement(screenWidth, ButtonWidth);

    _buttons.reserve(numOfButtons);
    _buttons.clear();

    for (auto buttonType : _buttonTypes) {
        _buttons.push_back(Button {
            buttonType,
            GetTextForButtonType(buttonType),
            SDL_Rect { buttonXPosition, buttonYPosition, ButtonWidth, ButtonHeight },
        });

        buttonYPosition += (ButtonHeight + ButtonSpacing);
    }
}

void MainMenu::MakeTextBlocksFromTexts(const std::vector<std::string>& additionalText)
{
    auto midPoint = GetCenteredPositionOfElement(_screen->ScreenWidth, TextBlockWidth);
    SDL_Rect textRect { midPoint, 100, TextBlockWidth, ButtonHeight };
    for (const auto& text : additionalText) {
        _additionalText.push_back(TextBlock { text, textRect });
        textRect.y += ButtonHeight + ButtonSpacing;
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

void MainMenu::TryHover(Vec2 position)
{
    _hoveredButton.reset();

    for (const auto& button : _buttons) {
        if (Contains(button.Position, position)) {
            _hoveredButton = button.Type;
        }
    }
}
