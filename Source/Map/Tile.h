#pragma once
#include "Sprite.h"
#include <glm/vec2.hpp>


class Tile: public Sprite {
public:
    Tile(glm::vec2 pos, glm::vec2 size, int index);

	glm::vec2 GetClip() const;
	glm::vec2 GetScale() const;
	
	void UpdateTextureClip();

	void UpdateClipX(float x);
	void UpdateClipY(float y);

	void UpdateScaleX(float x);
	void UpdateScaleY(float y);

private:
	glm::vec2 mClip = { 0.0f, 0.0f };
	glm::vec2 mScale = { 1.0f, 1.0f };
};
