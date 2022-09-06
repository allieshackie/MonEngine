#include <filesystem>
#include "Texture.h"
#include "Shader.h"
#include "RendererInstance.h"

#include "ResourceManager.h"

namespace fs = std::filesystem;

std::unordered_map<int, Texture*> ResourceManager::mTextures;
std::unordered_map<std::string, int> ResourceManager::mTextureIds;
LLGL::ResourceHeap* ResourceManager::mResourceHeap = nullptr;
int ResourceManager::mResourceIndex = -1;
std::vector<std::pair<int, Tile*>> ResourceManager::mSpritesList;

void ResourceManager::LoadAllTexturesFromFolder(LLGL::RenderSystem& renderer)
{
    int textureId = 0;
    for (const auto& entry : fs::directory_iterator(TEXTURE_FOLDER))
    {
        LoadTexture(renderer, entry.path().string(), entry.path().filename().string(), textureId);
        textureId++;
    }
}

void ResourceManager::LoadTexture(LLGL::RenderSystem& renderer, const std::string& filePath, const std::string& textureName, int textureId)
{
    if (mTextures.find(textureId) == mTextures.end())
    {
		const auto texture = new Texture(renderer, filePath);
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

void ResourceManager::CreateTile(const std::string& textureName, glm::vec2 pos, glm::vec2 size, glm::vec2 clip, glm::vec2 scale)
{
    const auto textureId = mTextureIds.find(textureName);
    if (textureId != mTextureIds.end())
    {
        const auto sprite = new Tile(pos, size, clip, scale);
        mSpritesList.emplace_back(std::make_pair(textureId->second, sprite));
    }
}

Tile* ResourceManager::GetLatestTile()
{
    return mSpritesList.front().second;
}

void ResourceManager::CreateLine(glm::vec4 line, glm::vec3 color)
{
    Line* debugLine = new Line();
    debugLine->pointA = { line.x, line.y };
    debugLine->pointB = { line.z, line.w };
    debugLine->color = color;

    RendererInstance::GetInstance()->AddDebugDrawToVB(debugLine);
}

void ResourceManager::CreateBox(glm::vec4 sideA, glm::vec4 sideB, glm::vec3 color)
{
    Box* debugBox = new Box();
    debugBox->pointA = { sideA.x, sideA.y };
    debugBox->pointB = { sideA.z, sideA.w };
    debugBox->pointC = { sideB.x, sideB.y };
    debugBox->pointD = { sideB.z, sideB.w };
    debugBox->color = color;

    RendererInstance::GetInstance()->AddDebugDrawToVB(debugBox);
}

void ResourceManager::CreateGrid(glm::vec4 sideA, glm::vec4 sideB, int rows, int columns, glm::vec3 color)
{
    Grid* debugGrid = new Grid();
    Box debugBox;
    debugBox.pointA = { sideA.x, sideA.y };
    debugBox.pointB = { sideA.z, sideA.w };
    debugBox.pointC = { sideB.x, sideB.y };
    debugBox.pointD = { sideB.z, sideB.w };
    debugGrid->mOutline = debugBox;
    debugGrid->color = color;

    float totalYDist = debugBox.pointB.y - debugBox.pointA.y;
    float yAmountToJump = totalYDist / rows;
    for (int i = 1; i < rows; i++)
    {
        Line line;
        line.pointA = { debugBox.pointA.x, debugBox.pointA.y + (i * yAmountToJump) };
        line.pointB = { debugBox.pointC.x, debugBox.pointC.y + (i * yAmountToJump) };
        debugGrid->mLines.push_back(line);
    }

    float totalXDist = debugBox.pointC.x - debugBox.pointA.x;
    float xAmountToJump = totalXDist / columns;
    for (int i = 1; i < columns; i++)
    {
        Line line;
        line.pointA = { debugBox.pointA.x + (i * xAmountToJump), debugBox.pointA.y };
        line.pointB = { debugBox.pointB.x + (i * xAmountToJump), debugBox.pointB.y };
        debugGrid->mLines.push_back(line);
    }

    RendererInstance::GetInstance()->AddDebugDrawToVB(debugGrid);
}

float ResourceManager::Normalize(float size)
{
    return (size - 1) / 99;
}

void ResourceManager::CreateResourceHeap(LLGL::RenderSystem& renderer, LLGL::PipelineLayout& pipelineLayout, LLGL::Buffer& constantBuffer)
{
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
    mResourceHeap = renderer.CreateResourceHeap(resourceHeapDesc);
}