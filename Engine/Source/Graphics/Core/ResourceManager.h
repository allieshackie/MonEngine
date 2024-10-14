#pragma once
#include <LLGL/LLGL.h>

#include "Model.h"
#include "Texture.h"

using GLuint = unsigned int;

class Animator;

class ResourceManager
{
public:
	ResourceManager() = default;

	void LoadAllResources(const LLGL::RenderSystemPtr& renderSystem);
	void InitModelVertexBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader) const;

	const std::vector<std::unique_ptr<Texture>>& GetTextures() const { return mTextures; }
	int GetTextureId(const std::string& textureName) const;
	LLGL::Texture& GetTexture(const std::string& textureName);
	LLGL::Sampler& GetSampler(const std::string& textureName);

	Model& GetModelFromId(const std::string& modelName) const;

	static bool CreateSimpleOpenGLTexture(const std::string& filename, GLuint* out_texture, int* out_width,
	                                      int* out_height);

private:
	void _LoadAllTexturesFromFolder(const LLGL::RenderSystemPtr& renderSystem);
	void _LoadAllModels();

	std::unordered_map<std::string, int> mModelIds;
	std::vector<std::unique_ptr<Model>> mModels;

	std::unordered_map<std::string, int> mTextureIds;
	std::vector<std::unique_ptr<Texture>> mTextures;
};
