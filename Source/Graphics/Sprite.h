#pragma once
#include "RenderObject.h"
#include <glm/mat4x4.hpp>


class Sprite : public RenderObject
{
public:
	Sprite() = default;
	// position: top left corner of sprite, size: width, height
	Sprite(int textureId, glm::vec2 pos, glm::vec2 size);

	void Draw() override;

	const glm::mat4& GetSpriteModelData() const;
	glm::mat4 GetTextureClip() const;
	glm::vec2 GetSize() const;
	glm::vec2 GetPosition() const;
	float GetRotation() const;

	void UpdateDrawData();
	void UpdatePosition(glm::vec2 pos);
	void UpdateSize(glm::vec2 size);
	void UpdateRotation(float rot);

protected:
	int mTextureId = 0;
	glm::mat4 mTextureClip = glm::mat4(1.0f);

private:
	glm::vec2 mPosition = {0, 0};
	glm::vec2 mSize = {1, 1};
	float mRotation = 0.0f;

	glm::mat4 mModel = glm::mat4(1.0);
};
