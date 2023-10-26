#pragma once

#include "Vec2.h"

#include <SDL.h>

#include <string>
#include <vector>

class Screen;

class SpriteAnimation {
public:
    SpriteAnimation(const std::string& animationDirectory, const Screen& screen);
    ~SpriteAnimation();

    void Draw(Vec2 location, int frameSize, double progress);

private:
    const Screen* _screen;

    std::vector<SDL_Texture*> _textures;

    void LoadAssets(const std::string& animationDirectory);
};
