#pragma once

#include "SpriteAnimation.h"
#include "Vec2.h"

#include <SDL.h>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Screen {
public:
    static std::unique_ptr<Screen> GetScreen();
    ~Screen();

    void BeginFrame() const;
    void DrawCell(Vec2 coords, int cellType, int sourceSize, int destinationSize) const;
    void DrawDestroyAnimation(Vec2 coords, int size, double progress);
    void DrawTexture(Vec2 coords, SDL_Texture* texture, SDL_Rect* sourceRect, SDL_Rect* destRect) const;
    void Present() const;

    SDL_Texture* LoadImage(const std::string& filePath) const;

private:
    bool Initialize();
    bool LoadAssets();

    static constexpr int ScreenWidth = 560;
    static constexpr int ScreenHeight = 560;
    static constexpr int AnimationTime = 5000;

    SDL_Window* _window = nullptr;
    SDL_Renderer* _renderer = nullptr;
    SDL_Texture* _renderTarget = nullptr;

    std::vector<SDL_Texture*> _assetImages;
    SDL_Texture* _backgroundImage = nullptr;

    std::unique_ptr<SpriteAnimation> _gravityAnimation;
};
