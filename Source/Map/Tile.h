#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "../Graphics/Texture.h"

class Tile {
public:
    //Initializes the variables
    Tile(int x, int y, int width, int height);

    //Get the collision box
    SDL_Rect getBox();

private:
    SDL_Rect mBox;
};