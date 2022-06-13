#include <filesystem>
#include "Texture.h"
#include "Shader.h"
#include "RendererInstance.h"

#include "ResourceManager.h"

namespace fs = std::filesystem;

std::unordered_map<int, Texture*> ResourceManager::mTextures;
std::unordered_map<std::string, int> ResourceManager::mTextureIds;
Shader* ResourceManager::mShader = nullptr;
LLGL::ResourceHeap* ResourceManager::mResourceHeap = nullptr;
LLGL::VertexFormat ResourceManager::mVertexFormat;
uint32_t ResourceManager::mResourceIndex = 0;
std::vector<std::unique_ptr<Sprite>> ResourceManager::mSpriteList;

void ResourceManager::LoadAllTexturesFromFolder()
{
    int textureId = 0;
    for (const auto& entry : fs::directory_iterator(TEXTURE_FOLDER))
    {
        LoadTexture(entry.path().string(), entry.path().filename().string(), textureId);
        textureId++;
    }
}

void ResourceManager::LoadTexture(const std::string& filePath, const std::string& textureName, int textureId)
{
    if (mTextures.find(textureId) == mTextures.end())
    {
		const auto texture = new Texture(filePath);
        mTextures[textureId] = texture;
        mTextureIds[textureName] = textureId;
    }
}

void ResourceManager::SetTexture(LLGL::CommandBuffer& commands, int textureId)
{	
    SetCurrentTexture(textureId);
    BindTexture(commands);
}

int ResourceManager::GetTextureId(const std::string& filePath)
{
    const auto& it = mTextureIds.find(filePath);
	if (it != mTextureIds.end())
	{
        return it->second;
	}

    return 0;
}

glm::vec2 ResourceManager::GetTextureSize(int textureId)
{
    const auto textureIt = mTextures.find(textureId);
	if (textureIt != mTextures.end())
	{
        return textureIt->second->GetTextureSize();
	}

    return { 0,0 };
}

void ResourceManager::BindTexture(LLGL::CommandBuffer& commands)
{
    commands.SetResourceHeap(*mResourceHeap, mResourceIndex);
}

void ResourceManager::SetCurrentTexture(int textureId)
{
    mResourceIndex = textureId;
}

const LLGL::VertexFormat& ResourceManager::GetVertexFormat()
{

    assert(!mVertexFormat.attributes.empty());
	
    return mVertexFormat;
}

void ResourceManager::LoadShaderProgram(const char* vertexFilePath, const char* fragmentFilePath)
{
    if (mShader == nullptr)
    {
        mVertexFormat.AppendAttribute({ "position", LLGL::Format::RG32Float });
        mVertexFormat.AppendAttribute({ "texCoord", LLGL::Format::RG32Float });

        mShader = new Shader(mVertexFormat, vertexFilePath, fragmentFilePath);
    }
}

LLGL::ShaderProgram& ResourceManager::GetShaderProgram()
{

    assert(mShader != nullptr);
    
    return mShader->GetShaderProgram();
}

void ResourceManager::RegisterSprite(Sprite* sprite)
{
    mSpriteList.emplace_back(sprite);
}

const std::vector<std::unique_ptr<Sprite>>& ResourceManager::GetSpriteList()
{
    return mSpriteList;
}

Sprite& ResourceManager::GetLatestSprite()
{
    return *mSpriteList.front();
}

float ResourceManager::Normalize(float size)
{
    return (size - 1) / 99;
}

void ResourceManager::CreateResourceHeap(LLGL::PipelineLayout& pipelineLayout)
{
    const auto& renderSystem = RendererInstance::GetInstance()->GetRendererSystem();
    LLGL::ResourceHeapDescriptor resourceHeapDesc;
    {
        resourceHeapDesc.pipelineLayout = &pipelineLayout;
        resourceHeapDesc.resourceViews.reserve(mTextures.size() * 2);
    	for (const auto& texture : mTextures)
    	{
            resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetSamplerData());
            resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetTextureData());
    		
    	}
  
    }
    mResourceHeap = renderSystem->CreateResourceHeap(resourceHeapDesc);
}