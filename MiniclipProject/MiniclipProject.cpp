//#define SDL_MAIN_HANDLED

#include "Game.h"

#include <SDL.h>

#include <iostream>

int main(int arg, char* argv[])
{
    Game game;
    game.RunMainLoop();

    return 0;
}
