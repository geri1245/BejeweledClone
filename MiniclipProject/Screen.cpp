#include "Screen.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include <array>
#include <iostream>

namespace {
constexpr size_t AssetTypeCount = 5;
std::array<std::string, AssetTypeCount> AssetNames = {
    "Assets/Color1.png",
    "Assets/Color2.png",
    "Assets/Color3.png",
    "Assets/Color4.png",
    "Assets/Color5.png",
};

static constexpr const char* BackgroundImagePath = "Assets/Background.png";
static constexpr const char* SpriteAnimationDirectory = "Assets/Gravity";
static constexpr const char* FontPath = "Assets/OpenSans.ttf";
static constexpr const char* BoldFontPath = "Assets/OpenSans-Bold.ttf";
static constexpr const char* MenuButtonImagePath = "Assets/MenuButton.png";

}

bool Screen::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        TerminateWithMessage(std::string("SDL could not initialize! SDL_Error: ") + SDL_GetError());
        return false;
    }

    int initFlags = IMG_INIT_PNG;
    if (IMG_Init(initFlags) != initFlags) {
        TerminateWithMessage(std::string("SDL_image could not initialize some image systems! SDL_image Error: ") + IMG_GetError());
        return false;
    }

    _window = SDL_CreateWindow("Jewels clone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
    if (!_window) {
        TerminateWithMessage(std::string("Window could not be created! SDL_Error: ") + SDL_GetError());
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, 0);
    if (!_renderer) {
        TerminateWithMessage(std::string("SDL renderer could not be created! SDL_Error: ") + SDL_GetError());
        return false;
    }

    if (TTF_Init() < 0) {
        TerminateWithMessage(std::string("SDL TTF could not be initialized:") + TTF_GetError());
    }

    _boldFont = TTF_OpenFont("Assets/OpenSans-bold.ttf", 24);
    _regularFont = TTF_OpenFont("Assets/OpenSans.ttf", 24);
    if (!_boldFont || !_regularFont) {
        TerminateWithMessage(std::string("Some fonts couldn't be loaded: ") + TTF_GetError());
    }

    return true;
}

void Screen::TerminateWithMessage(const std::string& errorText)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "An error occurred while running the application.", errorText.c_str(), _window);
    std::terminate();
}

bool Screen::LoadAssets()
{
    for (const auto& assetName : AssetNames) {
        auto image = LoadImage(assetName);

        if (!image) {
            return false;
        }

        _assetImages.push_back(std::move(image));
    }

    _backgroundImage = LoadImage(BackgroundImagePath);
    _menuButton = LoadImage(MenuButtonImagePath);

    if (!_backgroundImage) {
        return false;
    }

    _gravityAnimation = std::make_unique<SpriteAnimation>(SpriteAnimationDirectory, *this);

    return true;
}

std::unique_ptr<Screen> Screen::GetScreen()
{
    auto screen = std::make_unique<Screen>();
    if (screen->Initialize() && screen->LoadAssets()) {
        return screen;
    }

    return nullptr;
}

Screen::~Screen()
{
    SDL_DestroyRenderer(_renderer);

    SDL_DestroyWindow(_window);
    SDL_Quit();

    TTF_CloseFont(_regularFont);
    TTF_CloseFont(_boldFont);
}

void Screen::BeginFrame() const
{
    SDL_RenderCopy(_renderer, *_backgroundImage, nullptr, nullptr);
}

void Screen::DrawCell(Vec2 coords, int cellType, int sourceSize, int destinationSize) const
{
    SDL_Rect srcRect { 0, 0, sourceSize, sourceSize };
    SDL_Rect dstRect { coords.x, coords.y, destinationSize, destinationSize };
    SDL_RenderCopy(_renderer, *_assetImages[cellType], &srcRect, &dstRect);
}

void Screen::DrawDestroyAnimation(Vec2 coords, int size, double progress)
{
    _gravityAnimation->Draw(coords, size, progress);
}

void Screen::DrawTexture(const Texture& texture, const SDL_Rect* sourceRect, const SDL_Rect* destRect) const
{
    SDL_RenderCopy(_renderer, *texture, sourceRect, destRect);
}

void Screen::Present() const
{
    SDL_RenderPresent(_renderer);
}

void Screen::DrawText(const std::string& text, const SDL_Rect& textRect, bool isBold) const
{
    TTF_Font* font = isBold ? _boldFont : _regularFont;
    SDL_Color textColor { 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "Something went wrong: " << TTF_GetError() << std::endl;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Something went wrong: " << SDL_GetError() << std::endl;
    }

    SDL_FreeSurface(textSurface);

    int calculatedWidth, calculatedHeight;
    TTF_SizeText(font, text.c_str(), &calculatedWidth, &calculatedHeight);

    auto actualWidth = std::min(textRect.w, calculatedWidth);
    auto offset = (textRect.w - actualWidth) / 2;

    auto actualRect = textRect;
    actualRect.x += offset;
    actualRect.w = actualWidth;

    SDL_RenderCopy(_renderer, textTexture, nullptr, &actualRect);
    SDL_DestroyTexture(textTexture);
}

void Screen::DrawButton(const std::string& text, const SDL_Rect& coords) const
{
    DrawTexture(_menuButton, nullptr, &coords);
    DrawText(text, coords, true);
}

Texture Screen::LoadImage(const std::string& filePath) const
{
    // Load image at specified path
    Texture texture { IMG_LoadTexture(_renderer, filePath.c_str()) };
    if (!texture) {
        std::cerr << "Failed to load image texture. SDL_image Error: " << IMG_GetError() << std::endl;
        std::terminate();
    }

    return texture;
}
