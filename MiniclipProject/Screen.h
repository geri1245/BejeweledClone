#pragma once

#include <SDL.h>

#include <memory>
#include <string>
#include <vector>

class Screen {
public:
    static std::unique_ptr<Screen> GetScreen();
    ~Screen();

    void BeginFrame();
    void DrawCell(int row, int col, int cellType, int cellSize);
    void Present();

private:
    bool Initialize();
    bool LoadAssets();

    static constexpr int ScreenWidth = 560;
    static constexpr int ScreenHeight = 560;

    SDL_Window* _window = nullptr;
    SDL_Surface* _screenSurface = nullptr;

    std::vector<SDL_Surface*> _assetImages;

    SDL_Surface* LoadImage(const std::string& filePath);
};
