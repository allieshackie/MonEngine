#pragma once
#include <glm/mat4x4.hpp>

class Sprite
{
public:
	Sprite() = default;
	// position: top left corner of sprite, size: width, height
	Sprite(glm::vec2 pos, glm::vec2 size);
	
	const glm::mat4& GetSpriteModelData() const;
	glm::mat4 GetTextureClip() const;
	glm::vec2 GetSize() const;
	glm::vec2 GetPosition() const;
	float GetRotation() const;

	void UpdateDrawData();
	void UpdatePositionX(float x);
	void UpdatePositionY(float y);
	void UpdateSizeX(float x);
	void UpdateSizeY(float y);
	void UpdateRotation(float rot);

protected:
	glm::mat4 mTextureClip = glm::mat4(1.0f);
	
private:
	glm::vec2 mPosition = { 0, 0 };
	glm::vec2 mSize = { 1,1 };
	float mRotation = 180.0f;

	glm::mat4 mModel = glm::mat4(1.0);
};