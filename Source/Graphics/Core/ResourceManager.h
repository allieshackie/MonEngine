#pragma once
#include <LLGL/LLGL.h>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Vertex.h"

class DebugDrawable;
class Map;
class RenderObject;
class Renderer;
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
	void AddSprite(const std::string& textureName, glm::vec3 pos, glm::vec3 size);
	void AddTile(const std::string& textureName, glm::vec3 pos, glm::vec3 size,
	             glm::vec2 clip = {0.0f, 0.0f},
	             glm::vec2 scale = {1.0f, 1.0f});

	Sprite* CreateSprite(const std::string& textureName, glm::vec3 pos, glm::vec3 size);
	Tile* CreateTile(const std::string& textureName, glm::vec3 pos, glm::vec3 size,
	                 glm::vec2 clip = {0.0f, 0.0f},
	                 glm::vec2 scale = {1.0f, 1.0f});
	Map* CreateMap(glm::vec3 pos, const char* fileName);

	void AddRenderObjectToDrawList(RenderObject* obj);

	TriangleMesh LoadObjModel(std::vector<TexturedVertex>& vertices, const std::string& filename) const;

	const std::vector<RenderObject*>& GetDrawList();
	const std::vector<DebugDrawable*>& GetDebugDrawList();
	void ClearDebugDrawList();

	void CreateLine(const Renderer& renderer, glm::vec3 pointA, glm::vec3 pointB, glm::vec3 color);
	void CreateBox(const Renderer& renderer, glm::vec3 position, glm::vec3 size, glm::vec3 color);
	void CreateGrid(const Renderer& renderer, glm::vec3 position, glm::vec3 size, int rows, int columns,
	                glm::vec3 color);

	bool CreateSimpleOpenGLTexture(std::string& filename, GLuint* out_texture, int* out_width, int* out_height);

private:
	std::unordered_map<int, Texture*> mTextures;
	std::unordered_map<std::string, int> mTextureIds;
	std::vector<RenderObject*> mDrawList;
	std::vector<DebugDrawable*> mDebugDrawList;

	LLGL::ResourceHeap* mResourceHeap = nullptr;
	int mResourceIndex = 0;
};
