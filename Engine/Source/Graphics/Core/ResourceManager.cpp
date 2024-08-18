#include <glad/glad.h>
#include <filesystem>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Graphics/Util/stb_image.h"

#include "ResourceManager.h"

void ResourceManager::LoadAllResources(const LLGL::RenderSystemPtr& renderSystem)
{
	_LoadAllTexturesFromFolder(renderSystem);
	_LoadAllModels();
}

void ResourceManager::InitModelVertexBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader) const
{
	for (auto& model : mModels)
	{
		model->InitializeBuffers(renderSystem, shader);
	}
}

int ResourceManager::GetTextureId(const std::string& textureName) const
{
	const auto it = mTextureIds.find(textureName);
	if (it != mTextureIds.end())
	{
		return it->second;
	}

	return -1;
}

LLGL::Texture& ResourceManager::GetTexture(const std::string& textureName)
{
	const auto it = mTextureIds.find(textureName);
	if (it != mTextureIds.end())
	{
		return mTextures[it->second]->GetTextureData();
	}

	return mTextures[0]->GetTextureData();
}

Model& ResourceManager::GetModelFromId(const std::string& modelName) const
{
	const auto it = mModelIds.find(modelName);
	if (it != mModelIds.end())
	{
		return *mModels[it->second];
	}

	return *mModels[0];
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

void ResourceManager::_LoadAllTexturesFromFolder(const LLGL::RenderSystemPtr& renderSystem)
{
	int textureId = 0;
	for (const auto& entry : std::filesystem::directory_iterator(TEXTURES_FOLDER))
	{
		auto fullPath = entry.path().string();
		mTextures.push_back(std::make_unique<Texture>(renderSystem, fullPath));

		const size_t pos = fullPath.find(TEXTURES_FOLDER);

		// Check if the substring was found
		if (pos != std::string::npos)
		{
			// Erase the substring from the original string
			fullPath.erase(pos, std::strlen(TEXTURES_FOLDER));
		}
		mTextureIds[fullPath] = textureId++;
	}
}

void ResourceManager::_LoadAllModels()
{
	// Create an instance of the Importer class
	int modelId = 0;
	Assimp::Importer importer;
	for (const auto& entry : std::filesystem::directory_iterator(MODELS_FOLDER))
	{
		auto fullPath = entry.path().string();
		const aiScene* scene = importer.ReadFile(fullPath,
		                                         aiProcessPreset_TargetRealtime_Quality);
		// Check if there was errors with 
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << importer.GetErrorString() << std::endl;
		}

		mModels.push_back(std::make_unique<Model>(scene, entry.path().filename().string()));

		const size_t pos = fullPath.find(TEXTURES_FOLDER);

		// Check if the substring was found
		if (pos != std::string::npos)
		{
			// Erase the substring from the original string
			fullPath.erase(pos, std::strlen(TEXTURES_FOLDER));
		}
		mModelIds[fullPath] = modelId++;
	}
}
