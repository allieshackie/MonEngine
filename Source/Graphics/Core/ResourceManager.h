#pragma once
#include <LLGL/LLGL.h>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Vertex.h"

class RenderObject;
class Shader;
class Sprite;
class Texture;
class Tile;

static const char* TEXTURE_FOLDER = "../Data/Textures";

using GLuint = unsigned int;

class ResourceManager
{
public:
	ResourceManager() = default;
	~ResourceManager();

	void LoadAllTexturesFromFolder(LLGL::RenderSystem& renderer);
	void LoadTexture(LLGL::RenderSystem& renderer, const std::string& filePath, const std::string& textureName,
	                 int textureId);
	void SetTexture(LLGL::CommandBuffer& commands, int textureId);
	int GetTextureId(const std::string& filePath);
	glm::vec2 GetTextureSize(int textureId);

	Texture* GetTextureFromName(const std::string& filePath);

	void CreateResourceHeap(LLGL::RenderSystem& renderer, LLGL::PipelineLayout& pipelineLayout,
	                        LLGL::Buffer& constantBuffer);

	void BindTexture(LLGL::CommandBuffer& commands);
	void SetCurrentTexture(int textureId);

	// Helper used for editor
	void AddSprite(const std::string& textureName, glm::vec2 pos, glm::vec2 size);
	void AddTile(const std::string& textureName, glm::vec2 pos, glm::vec2 size,
	             glm::vec2 clip = {0.0f, 0.0f},
	             glm::vec2 scale = {1.0f, 1.0f});

	Sprite* CreateSprite(const std::string& textureName, glm::vec2 pos, glm::vec2 size);
	Tile* CreateTile(const std::string& textureName, glm::vec2 pos, glm::vec2 size,
	                 glm::vec2 clip = {0.0f, 0.0f},
	                 glm::vec2 scale = {1.0f, 1.0f});

	void AddRenderObjectToDrawList(RenderObject* obj);

	TriangleMesh LoadObjModel(std::vector<TexturedVertex>& vertices, const std::string& filename) const;

	const std::vector<RenderObject*>& GetDrawList();

	void CreateLine(glm::vec4 line, glm::vec3 color);
	void CreateBox(glm::vec4 sideA, glm::vec4 sideB, glm::vec3 color);
	void CreateGrid(glm::vec4 sideA, glm::vec4 sideB, int rows, int columns, glm::vec3 color);

	bool CreateSimpleOpenGLTexture(std::string& filename, GLuint* out_texture, int* out_width, int* out_height);

	static ResourceManager* GetInstance();

private:
	std::unordered_map<int, Texture*> mTextures;
	std::unordered_map<std::string, int> mTextureIds;
	std::vector<RenderObject*> mDrawList;

	LLGL::ResourceHeap* mResourceHeap = nullptr;
	int mResourceIndex = 0;

	static ResourceManager* mInstance;
};
