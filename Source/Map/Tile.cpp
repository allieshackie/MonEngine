#include "Tile.h"

Tile::Tile(int x, int y, int width, int height) 
{
    //Get the offsets
    mBox.x = x;
    mBox.y = y;

    //Set the collision box
    mBox.w = width;
    mBox.h = height;
}

SDL_Rect Tile::getBox()
{
	return mBox;
}
