#pragma once
#include <LLGL/LLGL.h>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Vertex.h"

struct DrawData;
class DebugDrawable;
class Map;
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

	int GetTextureId(const std::string& filePath);
	glm::vec2 GetTextureSize(int textureId);

	std::shared_ptr<Texture> GetTextureFromName(const std::string& filePath);

	const std::unordered_map<int, std::shared_ptr<Texture>>& getTextures();

	// Helper used for editor
	void AddSprite(const std::string& textureName, glm::vec3 pos, glm::vec3 size, std::string id = "");
	void AddSprite(const std::string& textureName, glm::vec3 pos, glm::vec3 size, glm::vec2 clip,
	               glm::vec2 scale, std::string id = "");
	void AddBox(glm::vec3 position, glm::vec3 size, std::string id = "");

	std::shared_ptr<DrawData>& GetDrawDataById(const std::string& id);

	TriangleMesh LoadObjModel(std::vector<TexturedVertex>& vertices, const std::string& filename) const;

	const std::vector<std::shared_ptr<DrawData>>& GetSpriteDrawList();
	const std::vector<std::shared_ptr<DrawData>>& GetDebugDrawList();

	void ClearDebugDrawList();

	bool CreateSimpleOpenGLTexture(std::string& filename, GLuint* out_texture, int* out_width, int* out_height);

private:
	std::unordered_map<int, std::shared_ptr<Texture>> mTextures;
	std::unordered_map<std::string, int> mTextureIds;

	std::vector<std::shared_ptr<DrawData>> mSpriteDrawList;
	std::vector<std::shared_ptr<DrawData>> mDebugDrawList;

	std::shared_ptr<DrawData> mEmptyData;
};
