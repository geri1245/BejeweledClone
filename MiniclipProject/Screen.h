#pragma once

#include <SDL.h>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Vec2.h"

class Screen {
public:
    static std::unique_ptr<Screen> GetScreen();
    ~Screen();

    void BeginFrame();
    void DrawCell(Vec2 coords, int cellType, int cellSize);
    void Present();

private:
    bool Initialize();
    bool LoadAssets();

    static constexpr int ScreenWidth = 560;
    static constexpr int ScreenHeight = 560;
    static constexpr int AnimationTime = 5000;

    SDL_Window* _window = nullptr;
    SDL_Surface* _screenSurface = nullptr;
    SDL_Renderer* _renderer = nullptr;
    SDL_Texture* _renderTarget = nullptr;

    std::vector<SDL_Texture*> _assetImages;

    SDL_Texture* LoadImage(const std::string& filePath);
};
