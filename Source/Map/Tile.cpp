#include <glm/ext/matrix_transform.hpp>

#include "Tile.h"

Tile::Tile(glm::vec2 pos, glm::vec2 size, int index) : Sprite(pos, size)
{
}

glm::vec2 Tile::GetClip() const 
{
	return mClip;
}

glm::vec2 Tile::GetScale() const
{
	return mScale;
}

void Tile::UpdateTextureClip()
{
	mTextureClip = glm::mat4(1.0f);
	mTextureClip = glm::translate(mTextureClip, glm::vec3(mClip.x, mClip.y, 0.0f));

	mTextureClip = glm::scale(mTextureClip, glm::vec3(mScale, 1.0f));
}

void Tile::UpdateClipX(float x)
{
	mClip.x = x;
}

void Tile::UpdateClipY(float y)
{
	mClip.y = y;
}

void Tile::UpdateScaleX(float x)
{
	mScale.x = x;
}

void Tile::UpdateScaleY(float y)
{
	mScale.y = y;
}
