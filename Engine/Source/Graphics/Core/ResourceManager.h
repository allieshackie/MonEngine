#pragma once
#include <LLGL/LLGL.h>
#include "Model.h"
#include "Texture.h"

using GLuint = unsigned int;

class ResourceManager
{
public:
	ResourceManager() = default;

	static std::vector<std::unique_ptr<Texture>> LoadAllTexturesFromFolder(
		const LLGL::RenderSystemPtr& renderSystem);

	static int GetTextureId(const std::string& textureName);

	static bool CreateSimpleOpenGLTexture(const std::string& filename, GLuint* out_texture, int* out_width,
	                                      int* out_height);

private:
	static std::unique_ptr<Texture> _LoadTexture(const LLGL::RenderSystemPtr& renderSystem,
	                                             const std::string& filePath);

	static std::unordered_map<std::string, int> mTextureIds;

	static std::vector<Vertex> mMapCubeVertices;
	static std::vector<std::uint32_t> mMapCubeIndices;
};
