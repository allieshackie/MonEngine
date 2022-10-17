#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Core/Renderer.h"

#include "Sprite.h"

Sprite::Sprite(int textureId, glm::vec2 pos, glm::vec2 size) : mTextureId(textureId), mPosition(pos), mSize(size)
{
	// NOTE: Previously set the position to be the center of the sprite
	// So that rotation was applied to the center, but this was causing
	// issues with map/grid alignment
	//mPosition = glm::vec2(pos.x + size.x / 2, pos.y + size.y / 2);
	UpdateDrawData();
}

void Sprite::Draw()
{
	const auto renderer = Renderer::GetInstance();
	renderer->SetTexture(mTextureId);
	UpdateDrawData();
	renderer->UpdateModelUniform(GetSpriteModelData());
	renderer->DrawSprite();
}

void Sprite::UpdateDrawData()
{
	mModel = glm::mat4(1.0f);

	const auto renderer = Renderer::GetInstance();
	const auto normalizedPos = renderer->MapToScreenCoordinates(mPosition);
	mModel = translate(mModel, glm::vec3(normalizedPos, 0.0f));

	const auto normalizedSize = renderer->MapToScreenCoordinates(mSize);
	mModel = translate(mModel, glm::vec3(0.5f * normalizedSize.x, 0.5f * normalizedSize.y, 0.0f));
	mModel = rotate(mModel, glm::radians(mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	mModel = translate(mModel, glm::vec3(-0.5f * normalizedSize.x, -0.5f * normalizedSize.y, 0.0f));

	mModel = scale(mModel, glm::vec3(normalizedSize, 1.0f));
}

void Sprite::UpdatePosition(glm::vec2 pos)
{
	mPosition = pos;
}

void Sprite::UpdateSize(glm::vec2 size)
{
	mSize = size;
}

const glm::mat4& Sprite::GetSpriteModelData() const
{
	return mModel;
}

glm::mat4 Sprite::GetTextureClip() const
{
	return mTextureClip;
}

glm::vec2 Sprite::GetSize() const
{
	return mSize;
}

glm::vec2 Sprite::GetPosition() const
{
	return mPosition;
}

float Sprite::GetRotation() const
{
	return mRotation;
}

void Sprite::UpdateRotation(float rot)
{
	mRotation = rot;
}
