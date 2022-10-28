#include <glm/vec3.hpp>

#include "Core/Renderer.h"

#include "Sprite.h"

Sprite::Sprite(int textureId, glm::vec3 pos, glm::vec3 size) : RenderObject(pos, size), mTextureId(textureId)
{
	// NOTE: Previously set the position to be the center of the sprite
	// So that rotation was applied to the center, but this was causing
	// issues with map/grid alignment
	//mPosition = glm::vec2(pos.x + size.x / 2, pos.y + size.y / 2);
	Update();
}

void Sprite::Draw()
{
	const auto renderer = Renderer::GetInstance();
	renderer->SetTexture(mTextureId);
	Update();
	renderer->UpdateProjectionViewModelUniform(mModel);
	renderer->DrawSprite();
}

const glm::mat4& Sprite::GetSpriteModelData() const
{
	return mModel;
}

glm::mat4 Sprite::GetTextureClip() const
{
	return mTextureClip;
}
