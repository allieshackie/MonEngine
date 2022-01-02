#pragma once
#include <LLGL/LLGL.h>

class Texture {
public:
	Texture(LLGL::RenderSystem& renderer, const std::string& path);
	~Texture();

	LLGL::Texture& GetTextureData() const;
	LLGL::Sampler& GetSamplerData() const;

private:
	bool LoadFromFile(LLGL::RenderSystem& renderer, const std::string& path);
	void CreateSampler(LLGL::RenderSystem& renderer);

	LLGL::Texture* mTexture = nullptr;
	LLGL::Sampler* mSampler = nullptr;
};