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

    _screenSurface = SDL_GetWindowSurface(_window);
    if (!_screenSurface) {
        std::cerr << "Screen surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
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
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Screen::BeginFrame()
{
    SDL_FillRect(_screenSurface, nullptr, SDL_MapRGB(_screenSurface->format, 255, 255, 255));
}

void Screen::DrawCell(int xCoord, int yCoord, int cellType, int cellSize)
{
    SDL_Rect rect { xCoord, yCoord, cellSize, cellSize };
    SDL_BlitSurface(_assetImages[cellType], nullptr, _screenSurface, &rect);
}

void Screen::Present()
{
    SDL_UpdateWindowSurface(_window);
}

SDL_Surface* Screen::LoadImage(const std::string& filePath)
{
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());
    if (!loadedSurface) {
        std::cerr << "Failed to load image. SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Surface* optimizedSurface = SDL_ConvertSurface(loadedSurface, _screenSurface->format, 0);
    SDL_FreeSurface(loadedSurface);

    if (!optimizedSurface) {
        std::cerr << "Failed to convert surface to the requested format! SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    return optimizedSurface;
}
