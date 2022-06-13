#pragma once
#include <LLGL/LLGL.h>
#include <unordered_map>
#include <glm/vec2.hpp>
#include "Sprite.h"

class Texture;
class Shader;

static const char* TEXTURE_FOLDER = "C:/dev/MonDev/Data/Textures";

const int PRIME_CONST = 31;

class ResourceManager
{
public:
	static void LoadAllTexturesFromFolder();
	static void LoadTexture(const std::string& filePath, const std::string& textureName, int textureId);
	static void SetTexture(LLGL::CommandBuffer& commands, int textureId);
	static int GetTextureId(const std::string& filePath);
	static glm::vec2 GetTextureSize(int textureId);

	static void CreateResourceHeap(LLGL::PipelineLayout& pipelineLayout);

	static void BindTexture(LLGL::CommandBuffer& commands);
	static void SetCurrentTexture(int textureId);
	static const LLGL::VertexFormat& GetVertexFormat();

	static void LoadShaderProgram(const char* vertexFilePath, const char* fragmentFilePath);
	static LLGL::ShaderProgram& GetShaderProgram();

	static void RegisterSprite(Sprite* sprite);
	static const std::vector<std::unique_ptr<Sprite>>& GetSpriteList();
	static Sprite& GetLatestSprite();

	static float Normalize(float size);
	
private:
	static std::unordered_map<int, Texture*> mTextures;
	static std::unordered_map<std::string, int> mTextureIds;
	static Shader* mShader;
	static LLGL::VertexFormat mVertexFormat;

	static LLGL::ResourceHeap* mResourceHeap;
	static uint32_t mResourceIndex;

	static std::vector<std::unique_ptr<Sprite>> mSpriteList;
};
