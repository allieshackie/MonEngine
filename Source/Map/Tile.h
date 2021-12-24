#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "../Graphics/Texture.h"

class Tile {
public:
    //Initializes the variables
    Tile(int clipPosX, int clipPosY, int width, int height, int screenPosX, int screenPosY);

    //Get the collision box
    SDL_Rect getBox();

    int getScreenPosX() { return mScreenPosX; }
    int getScreenPosY() { return mScreenPosY; }

    void updateScreenPosX(int x) {
        mScreenPosX += x;
    }

    void updateScreenPosY(int y) {
        mScreenPosY += y;
    }

private:
    SDL_Rect mBox;

    int mScreenPosX = 0;
    int mScreenPosY = 0;
};