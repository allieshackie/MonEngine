#pragma once
#include <LLGL/LLGL.h>
#include "Vertex.h"

class Map;
class Renderer;
class Texture;

using GLuint = unsigned int;

class ResourceManager
{
public:
	ResourceManager() = default;

	static std::vector<std::unique_ptr<Texture>> LoadAllTexturesFromFolder(
		const std::shared_ptr<LLGL::RenderSystem>& renderSystem);

	static int GetTextureId(const std::string& textureName);

	TriangleMesh LoadObjModel(std::vector<TexturedVertex>& vertices, const std::string& filename) const;

	static bool CreateSimpleOpenGLTexture(const std::string& filename, GLuint* out_texture, int* out_width,
	                                      int* out_height);
private:
	static std::unique_ptr<Texture> _LoadTexture(const std::shared_ptr<LLGL::RenderSystem>& renderSystem,
	                                             const std::string& filePath);

	static std::unordered_map<std::string, int> mTextureIds;
};
