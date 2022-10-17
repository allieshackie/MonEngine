#pragma once
#include <glm/vec2.hpp>
#include "Sprite.h"

class Tile : public Sprite
{
public:
	Tile(int textureId, glm::vec2 pos, glm::vec2 size, glm::vec2 clip = {0.0f, 0.0f},
	     glm::vec2 scale = {1.0f, 1.0f});

	void Draw() override;

	glm::vec2 GetClip() const;
	glm::vec2 GetScale() const;

	void UpdateTextureClip();

	void UpdateClip(glm::vec2 clip);
	void UpdateScale(glm::vec2 scale);

	void SetEmpty(bool empty);

private:
	glm::vec2 mClip = {0.0f, 0.0f};
	glm::vec2 mScale = {1.0f, 1.0f};
	bool mEmpty = false;
};
