#pragma once
#include <LLGL/LLGL.h>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Shader;
class Sprite;
class Texture;
class Tile;

static const char* TEXTURE_FOLDER = "../Data/Textures";

typedef unsigned int GLuint;

class ResourceManager
{
public:
	static void LoadAllTexturesFromFolder(LLGL::RenderSystem& renderer);
	static void LoadTexture(LLGL::RenderSystem& renderer, const std::string& filePath, const std::string& textureName, int textureId);
	static void SetTexture(LLGL::CommandBuffer& commands, int textureId);
	static int GetTextureId(const std::string& filePath);
	static glm::vec2 GetTextureSize(int textureId);

	static Texture* GetTextureFromName(const std::string& filePath);

	static void CreateResourceHeap(LLGL::RenderSystem& renderer, LLGL::PipelineLayout& pipelineLayout, LLGL::Buffer& constantBuffer);

	static void BindTexture(LLGL::CommandBuffer& commands);
	static void SetCurrentTexture(int textureId);

	// Helper used for editor
	static void CreateSprite(const std::string& textureName, glm::vec2 pos, glm::vec2 size);
	static const std::vector<std::pair<int, Tile*>>& GetSpritesList();
	static Sprite* GetLatestSprite();

	static void CreateTile(const std::string& textureName, glm::vec2 pos, glm::vec2 size, glm::vec2 clip = { 0.0f, 0.0f }, glm::vec2 scale = { 1.0f, 1.0f }, bool isInteractable = false);
	static Tile* GetLatestTile();

	static void CreateLine(glm::vec4 line, glm::vec3 color);
	static void CreateBox(glm::vec4 sideA, glm::vec4 sideB, glm::vec3 color);
	static void CreateGrid(glm::vec4 sideA, glm::vec4 sideB, int rows, int columns, glm::vec3 color);

	static bool CreateSimpleOpenGLTexture(std::string& filename, GLuint* out_texture, int* out_width, int* out_height);

	static float Normalize(float size);
	
private:
	static std::unordered_map<int, Texture*> mTextures;
	static std::unordered_map<std::string, int> mTextureIds;

	static LLGL::ResourceHeap* mResourceHeap;
	static int mResourceIndex;

	static std::vector<std::pair<int, Tile*>> mSpritesList;
};
