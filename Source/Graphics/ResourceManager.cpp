#include "Texture.h"
#include "Shader.h"

#include "ResourceManager.h"

std::map<int, Texture*> ResourceManager::mTextures;
Shader* ResourceManager::mShader = nullptr;
LLGL::ResourceHeap* ResourceManager::mResourceHeap = nullptr;
uint32_t ResourceManager::mResourceIndex = 0;

void ResourceManager::LoadTexture(LLGL::RenderSystem& renderer, const char* file, int textureId)
{
    if (mTextures.find(textureId) == mTextures.end())
    {
		Texture* texture = new Texture(renderer, file);
        mTextures.insert({ textureId, texture });
    }
}

void ResourceManager::SetTexture(LLGL::CommandBuffer& commands, int textureId)
{
    SetCurrentTexture(textureId);
    BindTexture(commands);
}

void ResourceManager::BindTexture(LLGL::CommandBuffer& commands)
{
    commands.SetResourceHeap(*mResourceHeap, mResourceIndex);
}

void ResourceManager::SetCurrentTexture(int textureId)
{
    mResourceIndex = textureId;
}

void ResourceManager::LoadShaderProgram(LLGL::RenderSystem& renderer, const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath)
{
    if (mShader == nullptr)
    {
        mShader = new Shader(renderer, vertexFormat, vertexFilePath, fragmentFilePath);
    }
}

LLGL::ShaderProgram& ResourceManager::GetShaderProgram()
{
    if (mShader == nullptr)
    {
        assert("Shader shouldn't be null!");
    }
	
    return mShader->GetShaderProgram();
}

void ResourceManager::CreateResourceHeap(LLGL::RenderSystem& renderer, LLGL::PipelineLayout& pipelineLayout)
{
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
    mResourceHeap = renderer.CreateResourceHeap(resourceHeapDesc);
}