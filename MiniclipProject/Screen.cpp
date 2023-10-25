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

    //_screenSurface = SDL_GetWindowSurface(_window);
    // if (!_screenSurface) {
    //    std::cerr << "Screen surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    //    return false;
    //}

    _renderer = SDL_CreateRenderer(_window, -1, 0);
    if (!_renderer) {
        std::cerr << "SDL renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    //_renderTarget = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ScreenWidth, ScreenHeight);
    // if (!_renderTarget) {
    //    std::cerr << "SDL render target could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    //    return false;
    //}

    // SDL_SetRenderTarget(_renderer, _renderTarget);

    SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);

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
    SDL_DestroyRenderer(_renderer);

    for (auto* texture : _assetImages) {
        SDL_DestroyTexture(texture);
    }

    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Screen::BeginFrame()
{
    SDL_RenderClear(_renderer);
    // SDL_FillRect(_screenSurface, nullptr, SDL_MapRGB(_screenSurface->format, 255, 255, 255));
}

void Screen::DrawCell(Vec2 coords, int cellType, int cellSize)
{
    SDL_Rect srcRect { 0, 0, cellSize, cellSize };
    SDL_Rect dstRect { coords.x, coords.y, cellSize, cellSize };
    // SDL_BlitSurface(_assetImages[cellType], nullptr, _screenSurface, &rect);
    SDL_RenderCopy(_renderer, _assetImages[cellType], &srcRect, &dstRect);
}

void Screen::Present()
{
    SDL_RenderPresent(_renderer);
}

SDL_Texture* Screen::LoadImage(const std::string& filePath)
{
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());
    if (!loadedSurface) {
        std::cerr << "Failed to load image. SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);

    if (!texture) {
        std::cerr << "Failed to convert surface to the requested format! SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    return texture;
}
