#pragma once
#include <LLGL/LLGL.h>
#include "Mesh.h"
#include "Texture.h"

using GLuint = unsigned int;

class ResourceManager
{
public:
	ResourceManager() = default;

	static std::vector<std::unique_ptr<Texture>> LoadAllTexturesFromFolder(
		const LLGL::RenderSystemPtr& renderSystem);

	static std::vector<std::unique_ptr<Mesh>> LoadAllMeshesFromFolder();

	static int GetTextureId(const std::string& textureName);

	static bool CreateSimpleOpenGLTexture(const std::string& filename, GLuint* out_texture, int* out_width,
	                                      int* out_height);

	static std::vector<TexturedVertex> GenerateTexturedCubeVertices()
	{
		return
		{
			//   x   y   z      nx  ny  nz      u  v
			// front
			{{-1, -1, -1}, {0, 0, -1}, {0, 1}},
			{{-1, 1, -1}, {0, 0, -1}, {0, 0}},
			{{1, 1, -1}, {0, 0, -1}, {1, 0}},
			{{1, -1, -1}, {0, 0, -1}, {1, 1}},

			// right
			{{1, -1, -1}, {+1, 0, 0}, {0, 1}},
			{{1, 1, -1}, {+1, 0, 0}, {0, 0}},
			{{1, 1, 1}, {+1, 0, 0}, {1, 0}},
			{{1, -1, 1}, {+1, 0, 0}, {1, 1}},

			// left
			{{-1, -1, 1}, {-1, 0, 0}, {0, 1}},
			{{-1, 1, 1}, {-1, 0, 0}, {0, 0}},
			{{-1, 1, -1}, {-1, 0, 0}, {1, 0}},
			{{-1, -1, -1}, {-1, 0, 0}, {1, 1}},

			// top
			{{-1, 1, -1}, {0, +1, 0}, {0, 1}},
			{{-1, 1, 1}, {0, +1, 0}, {0, 0}},
			{{1, 1, 1}, {0, +1, 0}, {1, 0}},
			{{1, 1, -1}, {0, +1, 0}, {1, 1}},

			// bottom
			{{-1, -1, 1}, {0, -1, 0}, {0, 1}},
			{{-1, -1, -1}, {0, -1, 0}, {0, 0}},
			{{1, -1, -1}, {0, -1, 0}, {1, 0}},
			{{1, -1, 1}, {0, -1, 0}, {1, 1}},

			// back
			{{1, -1, 1}, {0, 0, +1}, {0, 1}},
			{{1, 1, 1}, {0, 0, +1}, {0, 0}},
			{{-1, 1, 1}, {0, 0, +1}, {1, 0}},
			{{-1, -1, 1}, {0, 0, +1}, {1, 1}},
		};
	}

	static std::vector<std::uint32_t> GenerateTexturedCubeTriangleIndices()
	{
		return
		{
			0, 1, 2, 0, 2, 3, // front
			4, 5, 6, 4, 6, 7, // right
			8, 9, 10, 8, 10, 11, // left
			12, 13, 14, 12, 14, 15, // top
			16, 17, 18, 16, 18, 19, // bottom
			20, 21, 22, 20, 22, 23, // back
		};
	}

private:
	static std::unique_ptr<Texture> _LoadTexture(const LLGL::RenderSystemPtr& renderSystem,
	                                             const std::string& filePath);

	static std::unordered_map<std::string, int> mTextureIds;
};
