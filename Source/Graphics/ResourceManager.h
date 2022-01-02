#pragma once
#include <map>

class Texture;
class Shader;

class ResourceManager
{
public:
	static void LoadTexture(LLGL::RenderSystem& renderer, const char* file, int textureId);
	static void SetTexture(LLGL::CommandBuffer& commands, int textureId);

	static void CreateResourceHeap(LLGL::RenderSystem& renderer, LLGL::PipelineLayout& pipelineLayout);

	static void BindTexture(LLGL::CommandBuffer& commands);
	static void SetCurrentTexture(int textureId);

	static void LoadShaderProgram(LLGL::RenderSystem& renderer, const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath);
	static LLGL::ShaderProgram& GetShaderProgram();
	
private:
	static std::map<int, Texture*> mTextures;
	static Shader* mShader;

	static LLGL::ResourceHeap* mResourceHeap;
	static uint32_t mResourceIndex;

	// TODO: do we need a sampler for a texture?
	//LLGL::Sampler* msampler = nullptr;
};
