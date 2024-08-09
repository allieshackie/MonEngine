#include <glad/glad.h>
#include <filesystem>

#include "Graphics/Util/stb_image.h"

#include "ResourceManager.h"

std::unordered_map<std::string, int> ResourceManager::mTextureIds;

std::vector<Vertex> ResourceManager::mMapCubeVertices = {
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
std::vector<std::uint32_t> ResourceManager::mMapCubeIndices = {
	0, 1, 2, 0, 2, 3, // front
	4, 5, 6, 4, 6, 7, // right
	8, 9, 10, 8, 10, 11, // left
	12, 13, 14, 12, 14, 15, // top
	16, 17, 18, 16, 18, 19, // bottom
	20, 21, 22, 20, 22, 23, // back
};

std::vector<std::unique_ptr<Texture>> ResourceManager::LoadAllTexturesFromFolder(
	const LLGL::RenderSystemPtr& renderSystem)
{
	std::vector<std::unique_ptr<Texture>> textures;
	int textureId = 0;
	for (const auto& entry : std::filesystem::directory_iterator(TEXTURES_FOLDER))
	{
		auto fullPath = entry.path().string();
		textures.push_back(_LoadTexture(renderSystem, fullPath));

		const size_t pos = fullPath.find(TEXTURES_FOLDER);

		// Check if the substring was found
		if (pos != std::string::npos)
		{
			// Erase the substring from the original string
			fullPath.erase(pos, std::strlen(TEXTURES_FOLDER));
		}
		mTextureIds[fullPath] = textureId++;
	}

	return textures;
}

std::vector<Model> LoadAllModelsFromFolder()
{
	std::vector<Model> models;
	for (const auto& entry : std::filesystem::directory_iterator(MODELS_FOLDER))
	{
		auto fullPath = entry.path().string();
		models.emplace_back(fullPath, entry.path().filename().string());
	}

	return models;
}

int ResourceManager::GetTextureId(const std::string& textureName)
{
	const auto it = mTextureIds.find(textureName);
	if (it != mTextureIds.end())
	{
		return it->second;
	}

	return -1;
}

std::unique_ptr<Texture> ResourceManager::_LoadTexture(const LLGL::RenderSystemPtr& renderSystem,
                                                       const std::string& filePath)
{
	return std::make_unique<Texture>(renderSystem, filePath);
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool ResourceManager::CreateSimpleOpenGLTexture(const std::string& filename, GLuint* out_texture, int* out_width,
                                                int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	const std::string fullPath = TEXTURES_FOLDER + filename;
	unsigned char* image_data = stbi_load(fullPath.c_str(), &image_width, &image_height, nullptr, 4);
	if (image_data == nullptr)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}
