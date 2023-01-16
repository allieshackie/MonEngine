#include "Util/stb_image.h"

#include "Texture.h"

Texture::Texture(LLGL::RenderSystem& renderer, const std::string& path)
{
	LoadFromFile(renderer, path);
	CreateSampler(renderer);
}

Texture::~Texture()
{
	delete mTexture;
	delete mSampler;
}

LLGL::Texture& Texture::GetTextureData() const
{
	return *mTexture;
}

LLGL::Sampler& Texture::GetSamplerData() const
{
	return *mSampler;
}

glm::vec2 Texture::GetTextureSize() const
{
	return glm::vec2(mTextureWidth, mTextureHeight);
}

bool Texture::LoadFromFile(LLGL::RenderSystem& renderer, const std::string& path)
{
	// uncompressed texture
	int texComponents = 0;

	unsigned char* imageBuffer = stbi_load((path).c_str(), &mTextureWidth, &mTextureHeight, &texComponents, 0);
	if (!imageBuffer)
	{
		printf("Texture::LoadFromFile: stbi_load error!");
		return false;
	}

	// Initialize source image descriptor to upload image data onto GPU
	LLGL::SrcImageDescriptor imageDesc;
	{
		// Set color format depending on alpha channel
		imageDesc.format = (texComponents == 4 ? LLGL::ImageFormat::RGBA : LLGL::ImageFormat::RGB);

		// Set image data type (unsigned char = 8 bit unsigned int)
		imageDesc.dataType = LLGL::DataType::UInt8;

		imageDesc.data = imageBuffer;

		imageDesc.dataSize = mTextureWidth * mTextureHeight * texComponents;
	}

	{
		LLGL::TextureDescriptor texDesc;
		{
			texDesc.type = LLGL::TextureType::Texture2D;

			// texture hardware format: RGBA with normalize 8-bit unsigned char 
			texDesc.format = LLGL::Format::BGRA8UNorm;

			texDesc.extent = {static_cast<uint32_t>(mTextureWidth), static_cast<uint32_t>(mTextureHeight), 1u};

			// Generate all Mip map levels for texture (creates multiple sizes to be used for lower res)
			texDesc.miscFlags = LLGL::MiscFlags::GenerateMips;
		}

		mTexture = renderer.CreateTexture(texDesc, &imageDesc);
	}

	stbi_image_free(imageBuffer);

	return true;
}

void Texture::CreateSampler(LLGL::RenderSystem& renderer)
{
	// 1st sampler state with default settings
	// Create nearest sampler
	LLGL::SamplerDescriptor samplerDesc;
	{
		samplerDesc.minFilter = LLGL::SamplerFilter::Nearest;
		samplerDesc.magFilter = LLGL::SamplerFilter::Nearest;
		samplerDesc.mipMapFilter = LLGL::SamplerFilter::Nearest;
	}
	mSampler = renderer.CreateSampler(samplerDesc);
}
