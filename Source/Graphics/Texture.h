#pragma once
#include <LLGL/LLGL.h>
#include <glm/vec2.hpp>

class Texture {
public:
	Texture(LLGL::RenderSystem& renderer, const std::string& path);
	~Texture() = default;

	LLGL::Texture& GetTextureData() const;
	LLGL::Sampler& GetSamplerData() const;

	glm::vec2 GetTextureSize() const;

private:
	bool LoadFromFile(LLGL::RenderSystem& renderer, const std::string& path);
	void CreateSampler(LLGL::RenderSystem& renderer);

	LLGL::Texture* mTexture = nullptr;
	LLGL::Sampler* mSampler = nullptr;

	int mTextureWidth = 0;
	int mTextureHeight = 0;
};