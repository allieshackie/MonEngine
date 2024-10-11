#include <stb_image.h>

#include "Texture.h"

Texture::Texture(const LLGL::RenderSystemPtr& renderer, const std::string& path)
{
	_LoadFromFile(renderer, path);
	_CreateSampler(renderer);
}

Texture::Texture(const LLGL::RenderSystemPtr& renderer, const unsigned char* imageData, int width,
                 int height, bool singleChannel)
{
	if (singleChannel)
	{
		_CreateSingleChannelTextureFromData(renderer, imageData, width, height);
	}
	else
	{
		_CreateRGBAFromData(renderer, imageData, width, height);
	}
	_CreateSampler(renderer);
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
	return {mTextureWidth, mTextureHeight};
}

bool Texture::_LoadFromFile(const LLGL::RenderSystemPtr& renderer, const std::string& path)
{
	// uncompressed texture
	int texComponents = 0;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* imageBuffer = stbi_load((path).c_str(), &mTextureWidth, &mTextureHeight, &texComponents, 0);
	if (!imageBuffer)
	{
		printf("Texture::LoadFromFile: stbi_load error!");
		return false;
	}

	// Initialize source image descriptor to upload image data onto GPU
	LLGL::ImageView imageDesc;
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

		mTexture = renderer->CreateTexture(texDesc, &imageDesc);
	}

	stbi_image_free(imageBuffer);

	return true;
}

bool Texture::_CreateRGBAFromData(const LLGL::RenderSystemPtr& renderer, const unsigned char* imageData,
                                  int width, int height)
{
	mTextureWidth = width;
	mTextureHeight = height;

	// Initialize source image descriptor to upload image data onto GPU
	LLGL::ImageView imageDesc;
	{
		// Set color format depending on alpha channel
		imageDesc.format = LLGL::ImageFormat::RGB; // ImageFormat::RGB

		// Set image data type (unsigned char = 8 bit unsigned int)
		imageDesc.dataType = LLGL::DataType::UInt8;

		imageDesc.data = imageData;

		// TODO: * 4 for alpha channel as well? 
		imageDesc.dataSize = mTextureWidth * mTextureHeight * 3;
	}

	{
		LLGL::TextureDescriptor texDesc;
		{
			texDesc.type = LLGL::TextureType::Texture2D;

			// texture hardware format: RGBA with normalize 8-bit unsigned char
			texDesc.format = LLGL::Format::RGBA8UNorm;

			texDesc.extent = {static_cast<uint32_t>(mTextureWidth), static_cast<uint32_t>(mTextureHeight), 1u};

			// Generate all Mip map levels for texture (creates multiple sizes to be used for lower res)
			texDesc.miscFlags = LLGL::MiscFlags::GenerateMips;
		}

		mTexture = renderer->CreateTexture(texDesc, &imageDesc);
	}


	return true;
}

bool Texture::_CreateSingleChannelTextureFromData(const LLGL::RenderSystemPtr& renderer,
                                                  const unsigned char* imageData, int width, int height)
{
	mTextureWidth = width;
	mTextureHeight = height;

	const auto formatAttribs = LLGL::GetFormatAttribs(LLGL::Format::A8UNorm);

	// Initialize source image descriptor to upload image data onto GPU
	LLGL::ImageView imageDesc;
	{
		// Set color format depending on alpha channel
		imageDesc.format = formatAttribs.format;

		// Set image data type (unsigned char = 8 bit unsigned int)
		imageDesc.dataType = LLGL::DataType::UInt8;

		imageDesc.data = imageData;

		imageDesc.dataSize = static_cast<std::size_t>(mTextureWidth * mTextureHeight * 4);
	}

	{
		LLGL::TextureDescriptor texDesc;
		{
			texDesc.type = LLGL::TextureType::Texture2D;

			// texture hardware format: R with normalize 8-bit unsigned char
			texDesc.format = LLGL::Format::A8UNorm;

			texDesc.extent = {static_cast<uint32_t>(mTextureWidth), static_cast<uint32_t>(mTextureHeight), 1u};

			texDesc.bindFlags = LLGL::BindFlags::Sampled | LLGL::BindFlags::ColorAttachment;
		}

		mTexture = renderer->CreateTexture(texDesc, &imageDesc);
	}


	return true;
}

void Texture::_CreateSampler(const LLGL::RenderSystemPtr& renderer)
{
	// Set sampler filters to have different effects
	mSampler = renderer->CreateSampler(LLGL::SamplerDescriptor{});
}
