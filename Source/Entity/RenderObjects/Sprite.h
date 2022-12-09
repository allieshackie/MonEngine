#pragma once
#include "RenderObject.h"
#include <glm/mat4x4.hpp>

class Sprite : public RenderObject
{
public:
	// position: top left corner of sprite, size: width, height
	Sprite(int textureId, glm::vec3 pos, glm::vec3 size);

	void Draw(const Renderer& renderer, LLGL::CommandBuffer& commands) override;

	const glm::mat4& GetSpriteModelData() const;
	glm::mat4 GetTextureClip() const;

protected:
	int mTextureId = 0;
	glm::mat4 mTextureClip = glm::mat4(1.0f);
};
