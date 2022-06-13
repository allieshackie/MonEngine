#include "Tile.h"

Tile::Tile(int clipPosX, int clipPosY, int width, int height, int screenPosX, int screenPosY) : mScreenPosX(screenPosX), mScreenPosY(screenPosY)
{
    //Get the offsets
    mBox.x = clipPosX;
    mBox.y = clipPosY;

    //Set the collision box
    mBox.w = width;
    mBox.h = height;
}

SDL_Rect Tile::getBox() const
{
	return mBox;
}
