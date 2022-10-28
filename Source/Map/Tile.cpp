#include <glm/ext/matrix_transform.hpp>
#include "Core/Renderer.h"

#include "Tile.h"


Tile::Tile(int textureId, glm::vec3 pos, glm::vec3 size, glm::vec2 clip,
           glm::vec2 scale) : Sprite(textureId, pos, size), mClip(clip), mScale(scale)
{
	UpdateTextureClip();
}

void Tile::Draw()
{
	if (!mEmpty)
	{
		const auto renderer = Renderer::GetInstance();
		renderer->SetTexture(mTextureId);
		Update();
		UpdateTextureClip();
		renderer->UpdateProjectionViewModelUniform(mModel);
		renderer->UpdateTextureClipUniform(mTextureClip);
		renderer->DrawSprite();
	}
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
	mTextureClip = translate(mTextureClip, glm::vec3(mClip.x, mClip.y, 0.0f));

	mTextureClip = scale(mTextureClip, glm::vec3(mScale, 1.0f));
}

void Tile::UpdateClip(glm::vec2 clip)
{
	mClip = clip;
}

void Tile::UpdateScale(glm::vec2 scale)
{
	mScale = scale;
}

void Tile::SetEmpty(bool empty)
{
	mEmpty = empty;
}
