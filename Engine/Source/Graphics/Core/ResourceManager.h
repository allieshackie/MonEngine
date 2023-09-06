#pragma once
#include <LLGL/LLGL.h>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Vertex.h"

class Map;
class Renderer;
class Texture;

using GLuint = unsigned int;
using TextureMap = std::unordered_map<int, std::shared_ptr<Texture>>;

class ResourceManager
{
public:
	ResourceManager(std::string texturesFolderPath);
	~ResourceManager();

	void LoadAllTexturesFromFolder(LLGL::RenderSystem& renderer);
	void LoadTexture(LLGL::RenderSystem& renderer, const std::string& filePath, const std::string& textureName,
	                 int textureId);

	int GetTextureId(const std::string& filePath);
	glm::vec2 GetTextureSize(int textureId);

	const TextureMap& GetTextures();
	std::shared_ptr<Texture> GetTextureFromName(const std::string& filePath);


	TriangleMesh LoadObjModel(std::vector<TexturedVertex>& vertices, const std::string& filename) const;

	bool CreateSimpleOpenGLTexture(std::string& filename, GLuint* out_texture, int* out_width, int* out_height) const;
private:
	TextureMap mTextures;
	std::unordered_map<std::string, int> mTextureIds;

	std::string mTextureFolder;
};
