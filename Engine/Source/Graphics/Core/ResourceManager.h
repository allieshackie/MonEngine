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
	LLGL::Texture& GetTexture(int textureId) const;
	LLGL::Sampler& GetSampler(int textureId) const;

	Model& GetModelFromId(const std::string& modelName) const;

	static bool CreateSimpleOpenGLTexture(const std::string& filename, GLuint* out_texture, int* out_width,
	                                      int* out_height);

private:
	int _LoadNewTexture(const LLGL::RenderSystemPtr& renderSystem, const tinygltf::Image& image);
	void _LoadAllModels(const LLGL::RenderSystemPtr& renderSystem);

	// Process GLTF Models
	void _LoadModel(const LLGL::RenderSystemPtr& renderSystem, const std::string& fullPath, int modelId);
	void _ProcessMeshes(const LLGL::RenderSystemPtr& renderSystem, tinygltf::Model& model, Model& newModel);
	void _ProcessJointData(const tinygltf::Model& model, Model& newModel) const;
	void _ProcessAnimations(const tinygltf::Model& model, Model& newModel) const;

	std::unordered_map<std::string, int> mModelIds;
	std::vector<std::unique_ptr<Model>> mModels;

	std::unordered_map<std::string, int> mTextureIds;
	std::vector<std::unique_ptr<Texture>> mTextures;
	int textureNum = 0;
};
