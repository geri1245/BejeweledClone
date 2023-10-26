#include "Screen.h"

#include <SDL_image.h>

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

static constexpr const char* BackgroundImageName = "Assets/Background.png";
static constexpr const char* SpriteAnimationDirectory = "Assets/Gravity";

}

bool Screen::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int initFlags = IMG_INIT_PNG;
    if (IMG_Init(initFlags) != initFlags) {
        std::cerr << "SDL_image could not initialize some image systems! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    _window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
    if (!_window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, 0);
    if (!_renderer) {
        std::cerr << "SDL renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool Screen::LoadAssets()
{
    for (const auto& assetName : AssetNames) {
        auto image = LoadImage(assetName);

        if (!image) {
            return false;
        }

        _assetImages.push_back(image);
    }

    _backgroundImage = LoadImage(BackgroundImageName);

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

    for (auto* texture : _assetImages) {
        SDL_DestroyTexture(texture);
    }
    SDL_DestroyTexture(_backgroundImage);

    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Screen::BeginFrame() const
{
    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _backgroundImage, nullptr, nullptr);
}

void Screen::DrawCell(Vec2 coords, int cellType, int sourceSize, int destinationSize) const
{
    SDL_Rect srcRect { 0, 0, sourceSize, sourceSize };
    SDL_Rect dstRect { coords.x, coords.y, destinationSize, destinationSize };
    SDL_RenderCopy(_renderer, _assetImages[cellType], &srcRect, &dstRect);
}

void Screen::DrawDestroyAnimation(Vec2 coords, int size, double progress)
{
    _gravityAnimation->Draw(coords, size, progress);
}

void Screen::DrawTexture(Vec2 coords, SDL_Texture* texture, SDL_Rect* sourceRect, SDL_Rect* destRect) const
{
    SDL_RenderCopy(_renderer, texture, sourceRect, destRect);
}

void Screen::Present() const
{
    SDL_RenderPresent(_renderer);
}

SDL_Texture* Screen::LoadImage(const std::string& filePath) const
{
    // Load image at specified path
    SDL_Texture* texture = IMG_LoadTexture(_renderer, filePath.c_str());
    if (!texture) {
        std::cerr << "Failed to load image texture. SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    return texture;
}
