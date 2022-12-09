#include <glm/vec3.hpp>
#include "Core/Renderer.h"

#include "Sprite.h"

Sprite::Sprite(int textureId, glm::vec3 pos, glm::vec3 size) : RenderObject(pos, size), mTextureId(textureId)
{
	mVertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 0}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 1}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 0}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 1}}, // bottom right
	};
	Update();
}

void Sprite::Draw(const Renderer& renderer, LLGL::CommandBuffer& commands)
{
	renderer.SetTexture(mTextureId);
	Update();
	renderer.Update2DProjectionViewModelUniform(mModel);
	commands.Draw(4, 0);
}

const glm::mat4& Sprite::GetSpriteModelData() const
{
	return mModel;
}

glm::mat4 Sprite::GetTextureClip() const
{
	return mTextureClip;
}
