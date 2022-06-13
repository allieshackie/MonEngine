#pragma once

#include <SDL.h>

// SDL_RenderCopy(ren, sheet, &src, &dest);

/*
    cur_gid is a check if tile is empty?
    texCoordX = (cur_gid % (ts_width / tile_width)) * tile_width;
    texCoordY = (cur_gid / (ts_width / tile_width)) * tile_height;
*/

class Tile {
public:
    //Initializes the variables
    Tile(int clipPosX, int clipPosY, int width, int height, int screenPosX, int screenPosY);

    //Get the collision box
    SDL_Rect getBox() const;

    int getScreenPosX() const { return mScreenPosX; }
    int getScreenPosY() const { return mScreenPosY; }

    void updateScreenPosX(const int x) {
        mScreenPosX += x;
    }

    void updateScreenPosY(const int y) {
        mScreenPosY += y;
    }

private:
    SDL_Rect mBox = {0, 0, 0 ,0};

    int mScreenPosX = 0;
    int mScreenPosY = 0;
};