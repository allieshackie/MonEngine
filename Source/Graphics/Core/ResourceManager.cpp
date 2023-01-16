#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glad/glad.h>
#include <filesystem>
#include <sstream>
#include "Util/stb_image.h"

#include "Map.h"
#include "Texture.h"
#include "Renderer.h"
#include "Entity/RenderObjects/DebugDraw.h"
#include "Entity/RenderObjects/Tile.h"

#include "ResourceManager.h"

namespace fs = std::filesystem;

ResourceManager::~ResourceManager()
{
	mTextures.clear();
	mDrawList.clear();
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

const std::vector<std::shared_ptr<RenderObject>>& ResourceManager::GetDrawList()
{
	return mDrawList;
}

const std::vector<std::shared_ptr<DebugDrawable>>& ResourceManager::GetDebugDrawList()
{
	return mDebugDrawList;
}

void ResourceManager::ClearDebugDrawList()
{
	mDebugDrawList.clear();
}

void ResourceManager::AddSprite(const std::string& textureName, glm::vec3 pos, glm::vec3 size)
{
	const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
		auto sprite = std::make_shared<Sprite>(textureId->second, pos, size);
		//Renderer::GetInstance()->Add2DRenderObject(*sprite);
		mDrawList.emplace_back(std::move(sprite));
	}
}

void ResourceManager::AddTile(const std::string& textureName, glm::vec3 pos, glm::vec3 size,
                              glm::vec2 clip,
                              glm::vec2 scale)
{
	const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
		auto tile = std::make_shared<Tile>(textureId->second, pos, size, clip, scale);
		//Renderer::GetInstance()->Add2DRenderObject(*sprite);
		mDrawList.emplace_back(std::move(tile));
	}
}

std::shared_ptr<Sprite> ResourceManager::CreateSprite(const std::string& textureName, glm::vec3 pos, glm::vec3 size)
{
	const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
		auto sprite = std::make_shared<Sprite>(textureId->second, pos, size);
		return sprite;
	}

	return nullptr;
}

std::shared_ptr<Tile> ResourceManager::CreateTile(const std::string& textureName, glm::vec3 pos, glm::vec3 size,
                                                  glm::vec2 clip,
                                                  glm::vec2 scale)
{
	const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
		auto tile = std::make_shared<Tile>(textureId->second, pos, size, clip, scale);
		return tile;
	}
	return nullptr;
}

std::shared_ptr<Map> ResourceManager::CreateMap(glm::vec3 pos, const char* fileName)
{
	// TODO: Why is this pointer empty? 
	auto map = std::make_shared<Map>(*this, pos, fileName);
	//Renderer::GetInstance()->Add2DRenderObject(*map);
	mDrawList.emplace_back(map);
	return map;
}

void ResourceManager::AddRenderObjectToDrawList(std::shared_ptr<RenderObject> obj)
{
	mDrawList.emplace_back(obj);
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

void ResourceManager::CreateLine(const Renderer& renderer, glm::vec3 pointA, glm::vec3 pointB, glm::vec3 color)
{
	auto debugLine = std::make_shared<Line>(pointA, pointB, glm::vec3{0, 0, 0}, glm::vec3{1, 1, 1}, color);
	renderer.AddDebugRenderObject(*debugLine);
	mDebugDrawList.push_back(std::move(debugLine));
}

void ResourceManager::CreateBox(const Renderer& renderer, glm::vec3 position, glm::vec3 size, glm::vec3 color)
{
	auto debugBox = std::make_shared<Box>(position, size, color);
	renderer.AddDebugRenderObject(*debugBox);
	mDebugDrawList.push_back(std::move(debugBox));
}

void ResourceManager::CreateGrid(const Renderer& renderer, glm::vec3 position, glm::vec3 size, int rows, int columns,
                                 glm::vec3 color)
{
	auto debugBox = std::make_unique<Box>(position, size, color);
	std::vector<std::unique_ptr<Line>> lines;
	const float totalYDist = abs(debugBox->GetPointB().y - debugBox->GetPointA().y);
	const float yAmountToJump = totalYDist / rows;
	for (int i = 1; i < rows; i++)
	{
		auto line = std::make_unique<Line>(glm::vec3{
			                                   debugBox->GetPointA().x, debugBox->GetPointA().y - (i * yAmountToJump),
			                                   debugBox->GetPointA().z
		                                   },
		                                   glm::vec3{
			                                   debugBox->GetPointC().x, debugBox->GetPointC().y - (i * yAmountToJump),
			                                   debugBox->GetPointC().z
		                                   }, position, size, color);
		lines.push_back(std::move(line));
	}

	const float totalXDist = abs(debugBox->GetPointC().x - debugBox->GetPointA().x);
	const float xAmountToJump = totalXDist / columns;
	for (int i = 1; i < columns; i++)
	{
		auto line = std::make_unique<Line>(glm::vec3{
			                                   debugBox->GetPointA().x + (i * xAmountToJump), debugBox->GetPointA().y,
			                                   debugBox->GetPointA().z
		                                   },
		                                   glm::vec3{
			                                   debugBox->GetPointB().x + (i * xAmountToJump), debugBox->GetPointB().y,
			                                   debugBox->GetPointB().z
		                                   }, position, size, color);
		lines.push_back(std::move(line));
	}

	auto debugGrid = std::make_shared<Grid>(std::move(debugBox), lines, position, size, color);

	renderer.AddDebugRenderObject(*debugGrid);
	mDebugDrawList.push_back(std::move(debugGrid));
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
