#pragma once
#include <LLGL/LLGL.h>
#include <glm/vec2.hpp>

class Texture
{
public:
	Texture(const std::shared_ptr<LLGL::RenderSystem>& renderer, const std::string& path);
	Texture(const std::shared_ptr<LLGL::RenderSystem>& renderer, const unsigned char* imageData, int width, int height,
	        bool singleChannel = false);

	LLGL::Texture& GetTextureData() const;
	LLGL::Sampler& GetSamplerData() const;

	glm::vec2 GetTextureSize() const;

private:
	bool _LoadFromFile(const std::shared_ptr<LLGL::RenderSystem>& renderer, const std::string& path);
	bool _CreateRGBAFromData(const std::shared_ptr<LLGL::RenderSystem>& renderer, const unsigned char* imageData,
	                         int width, int height);
	bool _CreateSingleChannelTextureFromData(const std::shared_ptr<LLGL::RenderSystem>& renderer,
	                                         const unsigned char* imageData, int width, int height);
	void _CreateSampler(const std::shared_ptr<LLGL::RenderSystem>& renderer);

	LLGL::Texture* mTexture = nullptr;
	LLGL::Sampler* mSampler = nullptr;

	int mTextureWidth = 0;
	int mTextureHeight = 0;
};
