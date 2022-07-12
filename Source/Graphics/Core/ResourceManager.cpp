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
int ResourceManager::mResourceIndex = -1;
std::vector<std::pair<int, Tile*>> ResourceManager::mSpritesList;

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
	if (textureId != mResourceIndex)
	{
	    SetCurrentTexture(textureId);
	    BindTexture(commands);
	}
}

int ResourceManager::GetTextureId(const std::string& filePath)
{
	if (const auto& it = mTextureIds.find(filePath); it != mTextureIds.end())
	{
        return it->second;
	}

    return 0;
}

glm::vec2 ResourceManager::GetTextureSize(int textureId)
{
	if (const auto textureIt = mTextures.find(textureId); textureIt != mTextures.end())
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

void ResourceManager::CreateSprite(const std::string& textureName, glm::vec2 pos, glm::vec2 size)
{
    const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
	    //const auto sprite = new Sprite(pos, size);
        //mSpritesList.emplace_back(std::make_pair(textureId->second, sprite));
	}
}

const std::vector<std::pair<int, Tile*>>& ResourceManager::GetSpritesList()
{
    return mSpritesList;
}

Sprite* ResourceManager::GetLatestSprite()
{
    return mSpritesList.back().second;
}

void ResourceManager::CreateTile(const std::string& textureName, glm::vec2 pos, glm::vec2 size)
{
    const auto textureId = mTextureIds.find(textureName);
    if (textureId != mTextureIds.end())
    {
        const auto sprite = new Tile(pos, size, 0);
        mSpritesList.emplace_back(std::make_pair(textureId->second, sprite));
    }
}

Tile* ResourceManager::GetLatestTile()
{
    return mSpritesList.front().second;
}

float ResourceManager::Normalize(float size)
{
    return (size - 1) / 99;
}

void ResourceManager::CreateResourceHeap(LLGL::PipelineLayout& pipelineLayout, LLGL::Buffer& constantBuffer)
{
    const auto& renderSystem = RendererInstance::GetInstance()->GetRendererSystem();
    LLGL::ResourceHeapDescriptor resourceHeapDesc;
    {
        resourceHeapDesc.pipelineLayout = &pipelineLayout;
        resourceHeapDesc.resourceViews.reserve(mTextures.size() * 3);
    	
    	for (const auto& texture : mTextures)
    	{
            resourceHeapDesc.resourceViews.emplace_back(&constantBuffer);
            resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetTextureData());
            resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetSamplerData());
    	}
  
    }
    mResourceHeap = renderSystem->CreateResourceHeap(resourceHeapDesc);
}