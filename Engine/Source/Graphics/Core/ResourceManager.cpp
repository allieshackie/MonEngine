#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glad/glad.h>
#include <filesystem>
#include <sstream>
#include "Graphics/Util/stb_image.h"
#include "Graphics/Texture.h"
#include "Graphics/Core/Renderer.h"

#include "ResourceManager.h"

namespace fs = std::filesystem;

ResourceManager::~ResourceManager()
{
	mTextures.clear();
}

void ResourceManager::LoadAllTexturesFromFolder(LLGL::RenderSystem& renderer)
{
	int textureId = 0;
	for (const auto& entry : fs::directory_iterator(TEXTURE_FOLDER))
	{
		LoadTexture(renderer, entry.path().string(), entry.path().filename().string(), textureId);
		textureId++;
	}
}

void ResourceManager::LoadTexture(LLGL::RenderSystem& renderer, const std::string& filePath,
                                  const std::string& textureName, int textureId)
{
	if (mTextures.find(textureId) == mTextures.end())
	{
		auto texture = std::make_shared<Texture>(renderer, filePath);
		mTextures[textureId] = std::move(texture);
		mTextureIds[textureName] = textureId;
	}
}

int ResourceManager::GetTextureId(const std::string& filePath)
{
	if (const auto& it = mTextureIds.find(filePath); it != mTextureIds.end())
	{
		return it->second;
	}

	return 0;
}

glm::vec2 ResourceManager::GetTextureSize(int textureId)
{
	if (const auto textureIt = mTextures.find(textureId); textureIt != mTextures.end())
	{
		return textureIt->second->GetTextureSize();
	}

	return {0, 0};
}

std::shared_ptr<Texture> ResourceManager::GetTextureFromName(const std::string& filePath)
{
	const auto textureId = GetTextureId(filePath);
	const auto textureIt = mTextures.find(textureId);

	assert(textureIt != mTextures.end());

	return textureIt->second;
}

const std::unordered_map<int, std::shared_ptr<Texture>>& ResourceManager::getTextures()
{
	return mTextures;
}

TriangleMesh ResourceManager::LoadObjModel(std::vector<TexturedVertex>& vertices, const std::string& filename) const
{
	// Read obj file
	std::ifstream file(filename);
	if (!file.good())
		throw std::runtime_error("failed to load model from file: \"" + filename + "\"");

	// Initialize triangle mesh
	TriangleMesh mesh;
	mesh.firstVertex = static_cast<std::uint32_t>(vertices.size());

	std::vector<glm::vec3> coords, normals;
	std::vector<glm::vec2> texCoords;

	std::string line;
	// Read each line
	while (std::getline(file, line))
	{
		std::stringstream s;
		s << line;

		// Parse line
		std::string mode;
		s >> mode;

		if (mode == "v")
		{
			glm::vec3 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			coords.push_back(v);
		}
		else if (mode == "vt")
		{
			glm::vec2 t;
			s >> t.x;
			s >> t.y;
			texCoords.push_back(t);
		}
		else if (mode == "vn")
		{
			glm::vec3 n;
			s >> n.x;
			s >> n.y;
			s >> n.z;
			normals.push_back(n);
		}
		else if (mode == "f")
		{
			unsigned int v = 0, vt = 0, vn = 0;

			for (int i = 0; i < 3; ++i)
			{
				// Read vertex index
				s >> v;

				// Read texture-coordinate index
				if (texCoords.empty())
					s.ignore(2);
				else
				{
					s.ignore(1);
					s >> vt;
					s.ignore(1);
				}

				// Read normal index
				s >> vn;

				// Add vertex to mesh
				vertices.push_back(
					{
						coords[v - 1],
						(vn - 1 < normals.size() ? normals[vn - 1] : glm::vec3{}),
						(vt - 1 < texCoords.size() ? texCoords[vt - 1] : glm::vec2{})
					}
				);
				mesh.numVertices++;
			}
		}
	}

	return mesh;
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool ResourceManager::CreateSimpleOpenGLTexture(std::string& filename, GLuint* out_texture, int* out_width,
                                                int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	filename.insert(0, "/");
	const std::string fullPath = TEXTURE_FOLDER + filename;
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
