#include "Texture.h"

#include "stb_image.h"
#include "LLGL/Utility.h"

static std::string TEXTURE_PATH = "C:/dev/MonDev/Data/Textures/";

Texture::Texture(LLGL::RenderSystem& renderer, const std::string& path) {
	LoadFromFile(renderer, path);
    CreateSampler(renderer);
}

Texture::~Texture() {
}

LLGL::Texture& Texture::GetTextureData() const
{
    return *mTexture;
}

LLGL::Sampler& Texture::GetSamplerData() const
{
    return *mSampler;
}

bool Texture::LoadFromFile(LLGL::RenderSystem& renderer, const std::string& path) {
	std::string fullPath = TEXTURE_PATH;
	fullPath.append(path);

    // uncompressed texture
    int texWidth = 0, texHeight = 0, texComponents = 0;

    unsigned char* imageBuffer = stbi_load((fullPath).c_str(), &texWidth, &texHeight, &texComponents, 0);
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

        imageDesc.dataSize = static_cast<size_t>(texWidth * texHeight * texComponents);
    }

    {
        LLGL::TextureDescriptor texDesc;
        {
            texDesc.type = LLGL::TextureType::Texture2D;

            // texture hardware format: RGBA with normalize 8-bit unsigned char 
            texDesc.format = LLGL::Format::BGRA8UNorm;

            texDesc.extent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1u };

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
    LLGL::SamplerDescriptor samplerDesc;
    mSampler = renderer.CreateSampler(samplerDesc);

    /*
    // 2nd sampler state with MIP-map bias
    samplerDesc.mipMapLODBias = 3.0f; // TODO: what is this number?
    sampler[1] = mRenderer->CreateSampler(samplerDesc);

    // 3rd with nearest filtering
    samplerDesc.minFilter = LLGL::SamplerFilter::Nearest;
    sampler[2] = mRenderer->CreateSampler(samplerDesc);

    // 4th with clamped texture wrap mode
    samplerDesc.minFilter = LLGL::SamplerFilter::Linear;
    samplerDesc.mipMapLODBias = 0.0f;
    samplerDesc.addressModeU = LLGL::SamplerAddressMode::MirrorOnce;
    samplerDesc.addressModeV = LLGL::SamplerAddressMode::Border;
    sampler[3] = mRenderer->CreateSampler(samplerDesc);

    // 5th with mirrored texture wrap mode
    samplerDesc.addressModeU = LLGL::SamplerAddressMode::Mirror;
    samplerDesc.addressModeV = LLGL::SamplerAddressMode::Mirror;
    sampler[4] = mRenderer->CreateSampler(samplerDesc);
     */
}
