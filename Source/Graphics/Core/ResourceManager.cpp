#include <glad/glad.h>
#include <filesystem>
#include <sstream>
#include "Util/stb_image.h"
#include "Renderer.h"
#include "Texture.h"
#include "Tile.h"

#include "ResourceManager.h"

namespace fs = std::filesystem;

ResourceManager* ResourceManager::mInstance{nullptr};

ResourceManager::~ResourceManager()
{
	for (const auto& tex : mTextures)
	{
		delete(tex.second);
	}
	mTextures.clear();

	for (const auto draw : mDrawList)
	{
		delete(draw);
	}
	mDrawList.clear();

	delete(mInstance);
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
		const auto texture = new Texture(renderer, filePath);
		mTextures[textureId] = texture;
		mTextureIds[textureName] = textureId;
	}
}

void ResourceManager::SetTexture(LLGL::CommandBuffer& commands, int textureId)
{
	if (textureId != mResourceIndex)
	{
		SetCurrentTexture(textureId);
		BindTexture(commands);
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

Texture* ResourceManager::GetTextureFromName(const std::string& filePath)
{
	const auto textureId = GetTextureId(filePath);
	const auto textureIt = mTextures.find(textureId);

	assert(textureIt != mTextures.end());

	return textureIt->second;
}

void ResourceManager::BindTexture(LLGL::CommandBuffer& commands)
{
	commands.SetResourceHeap(*mResourceHeap, mResourceIndex);
}

void ResourceManager::SetCurrentTexture(int textureId)
{
	mResourceIndex = textureId;
}

const std::vector<RenderObject*>& ResourceManager::GetDrawList()
{
	return mDrawList;
}

void ResourceManager::AddSprite(const std::string& textureName, glm::vec2 pos, glm::vec2 size)
{
	const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
		const auto sprite = new Sprite(textureId->second, pos, size);
		mDrawList.emplace_back(sprite);
	}
}

void ResourceManager::AddTile(const std::string& textureName, glm::vec2 pos, glm::vec2 size,
                              glm::vec2 clip,
                              glm::vec2 scale)
{
	const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
		const auto sprite = new Tile(textureId->second, pos, size, clip, scale);
		mDrawList.emplace_back(sprite);
	}
}

Sprite* ResourceManager::CreateSprite(const std::string& textureName, glm::vec2 pos, glm::vec2 size)
{
	const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
		const auto sprite = new Sprite(textureId->second, pos, size);
		return sprite;
	}

	return nullptr;
}

Tile* ResourceManager::CreateTile(const std::string& textureName, glm::vec2 pos, glm::vec2 size,
                                  glm::vec2 clip,
                                  glm::vec2 scale)
{
	const auto textureId = mTextureIds.find(textureName);
	if (textureId != mTextureIds.end())
	{
		const auto tile = new Tile(textureId->second, pos, size, clip, scale);
		return tile;
	}
	return nullptr;
}

void ResourceManager::AddRenderObjectToDrawList(RenderObject* obj)
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

void ResourceManager::CreateLine(glm::vec4 line, glm::vec3 color)
{
	auto debugLine = new Line();
	debugLine->pointA = {line.x, line.y};
	debugLine->pointB = {line.z, line.w};
	debugLine->color = color;

	Renderer::GetInstance()->AddDebugDrawToVB(debugLine);
}

void ResourceManager::CreateBox(glm::vec4 sideA, glm::vec4 sideB, glm::vec3 color)
{
	auto debugBox = new Box();
	debugBox->pointA = {sideA.x, sideA.y};
	debugBox->pointB = {sideA.z, sideA.w};
	debugBox->pointC = {sideB.x, sideB.y};
	debugBox->pointD = {sideB.z, sideB.w};
	debugBox->color = color;

	Renderer::GetInstance()->AddDebugDrawToVB(debugBox);
}

void ResourceManager::CreateGrid(glm::vec4 sideA, glm::vec4 sideB, int rows, int columns, glm::vec3 color)
{
	auto debugGrid = new Grid();
	Box debugBox;
	debugBox.pointA = {sideA.x, sideA.y};
	debugBox.pointB = {sideA.z, sideA.w};
	debugBox.pointC = {sideB.x, sideB.y};
	debugBox.pointD = {sideB.z, sideB.w};
	debugGrid->mOutline = debugBox;
	debugGrid->color = color;

	float totalYDist = debugBox.pointB.y - debugBox.pointA.y;
	float yAmountToJump = totalYDist / rows;
	for (int i = 1; i < rows; i++)
	{
		Line line;
		line.pointA = {debugBox.pointA.x, debugBox.pointA.y + (i * yAmountToJump)};
		line.pointB = {debugBox.pointC.x, debugBox.pointC.y + (i * yAmountToJump)};
		debugGrid->mLines.push_back(line);
	}

	float totalXDist = debugBox.pointC.x - debugBox.pointA.x;
	float xAmountToJump = totalXDist / columns;
	for (int i = 1; i < columns; i++)
	{
		Line line;
		line.pointA = {debugBox.pointA.x + (i * xAmountToJump), debugBox.pointA.y};
		line.pointB = {debugBox.pointB.x + (i * xAmountToJump), debugBox.pointB.y};
		debugGrid->mLines.push_back(line);
	}

	Renderer::GetInstance()->AddDebugDrawToVB(debugGrid);
}

ResourceManager* ResourceManager::GetInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new ResourceManager();
	}
	return mInstance;
}

void ResourceManager::CreateResourceHeap(LLGL::RenderSystem& renderer, LLGL::PipelineLayout& pipelineLayout,
                                         LLGL::Buffer& constantBuffer)
{
	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = &pipelineLayout;
		resourceHeapDesc.resourceViews.reserve(mTextures.size() * 3);

		for (const auto& texture : mTextures)
		{
			resourceHeapDesc.resourceViews.emplace_back(&constantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetTextureData());
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetSamplerData());
		}
	}
	mResourceHeap = renderer.CreateResourceHeap(resourceHeapDesc);
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
