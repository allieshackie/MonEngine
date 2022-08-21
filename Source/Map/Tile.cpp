#include <glm/ext/matrix_transform.hpp>

#include "Tile.h"

Tile::Tile(glm::vec2 pos, glm::vec2 size, glm::vec2 clip, glm::vec2 scale) : Sprite(pos, size), mClip(clip), mScale(scale)
{
	UpdateTextureClip();
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

void Tile::UpdateClip(glm::vec2 clip)
{
	mClip = clip;
}

void Tile::UpdateScale(glm::vec2 scale)
{
	mScale = scale;
}

