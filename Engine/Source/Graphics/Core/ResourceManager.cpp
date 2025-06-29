#include <glad/glad.h>
#include <filesystem>
#include <stb_image.h>

#include "ResourceManager.h"

#include <glm/gtc/type_ptr.hpp>

#include "Util/gltfHelpers.h"

void ResourceManager::LoadAllResources(const LLGL::RenderSystemPtr& renderSystem)
{
	_LoadAllModels(renderSystem);
}

void ResourceManager::InitModelVertexBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader) const
{
	for (auto& model : mModels)
	{
		model->InitializeBuffers(renderSystem, shader);
	}
}

LLGL::Texture& ResourceManager::GetTexture(int textureId) const
{
	if (textureId == -1)
	{
		return mTextures[0]->GetTextureData();
	}
	return mTextures[textureId]->GetTextureData();
}

LLGL::Sampler& ResourceManager::GetSampler(int textureId) const
{
	if (textureId == -1)
	{
		return mTextures[0]->GetSamplerData();
	}
	return mTextures[textureId]->GetSamplerData();
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

int ResourceManager::_LoadNewTexture(const LLGL::RenderSystemPtr& renderSystem, const tinygltf::Image& image)
{
	mTextures.push_back(std::make_unique<Texture>(renderSystem, image));
	mTextureIds[image.name] = textureNum++;
	return mTextureIds[image.name];
}

void ResourceManager::_LoadAllModels(const LLGL::RenderSystemPtr& renderSystem)
{
	int modelId = 0;
	for (const auto& entry : std::filesystem::directory_iterator(MODELS_FOLDER))
	{
		auto fullPath = entry.path().string();
		_LoadModel(renderSystem, fullPath, modelId);

		const size_t pos = fullPath.find(MODELS_FOLDER);

		// Check if the substring was found
		if (pos != std::string::npos)
		{
			// Erase the substring from the original string
			fullPath.erase(pos, std::strlen(MODELS_FOLDER));
		}
		mModelIds[entry.path().filename().string()] = modelId++;
	}
}

void ResourceManager::_LoadModel(const LLGL::RenderSystemPtr& renderSystem, const std::string& fullPath, int modelId)
{
	tinygltf::TinyGLTF gltf_ctx;
	std::string err;
	std::string warn;

	tinygltf::Model model;

	bool result = gltf_ctx.LoadASCIIFromFile(&model, &err, &warn, fullPath);

	if (!warn.empty())
	{
		printf("Warn: %s\n", warn.c_str());
	}

	if (!err.empty())
	{
		printf("Err: %s\n", err.c_str());
	}

	if (!result)
	{
		printf("Failed to parse glTF\n");
	}

	auto newModel = std::make_unique<Model>(modelId, model.nodes.size());

	_ProcessMeshes(renderSystem, model, *newModel);
	_ProcessJointData(model, *newModel);
	_ProcessAnimations(model, *newModel);

	mModels.push_back(std::move(newModel));
}

void ResourceManager::_ProcessMeshes(const LLGL::RenderSystemPtr& renderSystem, tinygltf::Model& model,
                                     Model& newModel)
{
	for (unsigned int i = 0; i < model.meshes.size(); i++)
	{
		const auto& mesh = model.meshes[i];

		auto meshData = new MeshData();
		meshData->mName = mesh.name;

		for (const auto& primitive : mesh.primitives)
		{
			const float* positions = nullptr;
			int posCount = 0;
			if (const auto& it = primitive.attributes.find("POSITION"); it != primitive.attributes.end())
			{
				positions = gltfHelpers::GetgltfBuffer<const float*>(model, it->second, posCount);
			}

			// Retrieve normals (if they exist)
			const float* normals = nullptr;
			if (const auto& it = primitive.attributes.find("NORMAL"); it != primitive.attributes.end())
			{
				normals = gltfHelpers::GetgltfBuffer<const float*>(model, it->second);
			}

			// Retrieve texture coordinates (if they exist)
			const float* texCoords = nullptr;
			if (const auto& it = primitive.attributes.find("TEXCOORD_0"); it != primitive.attributes.end())
			{
				texCoords = gltfHelpers::GetgltfBuffer<const float*>(model, it->second);
			}

			// Retrieve bone IDs (JOINTS_0)
			const unsigned char* joints = nullptr;
			if (const auto& it = primitive.attributes.find("JOINTS_0"); it != primitive.attributes.end())
			{
				joints = gltfHelpers::GetgltfBuffer<const unsigned char*>(model, it->second);
			}

			// Retrieve bone weights (WEIGHTS_0)
			const float* weights = nullptr;
			if (const auto& it = primitive.attributes.find("WEIGHTS_0"); it != primitive.attributes.end())
			{
				weights = gltfHelpers::GetgltfBuffer<const float*>(model, it->second);
			}

			glm::vec3 minBounds(std::numeric_limits<float>::max());
			glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

			meshData->mVertices.resize(posCount);
			for (int vertId = 0; vertId < posCount; vertId++)
			{
				// Position
				meshData->mVertices[vertId].position = glm::make_vec3(&positions[vertId * VEC3_STEP]);

				minBounds = glm::min(minBounds, meshData->mVertices[vertId].position);
				maxBounds = glm::max(maxBounds, meshData->mVertices[vertId].position);

				// Normal (optional)
				if (normals)
				{
					meshData->mVertices[vertId].normal = glm::make_vec3(&normals[vertId * VEC3_STEP]);
				}
				else
				{
					meshData->mVertices[vertId].normal = glm::vec3(0.0f, 0.0f, 0.0f); // Default if no normal provided
				}

				// Texture coordinates (optional)
				if (texCoords)
				{
					meshData->mVertices[vertId].texCoord = glm::make_vec2(&texCoords[vertId * VEC2_STEP]);
					meshData->mVertices[vertId].texCoord.y = 1.0f - meshData->mVertices[vertId].texCoord.y;
				}
				else
				{
					meshData->mVertices[vertId].texCoord = glm::vec2(0.0f, 0.0f); // Default if no tex coords provided
				}

				// Bone IDs (optional)
				if (joints)
				{
					auto jointVec = glm::make_vec4(&joints[vertId * VEC4_STEP]);
					meshData->mVertices[vertId].boneIds[0] = model.skins[0].joints[jointVec[0]];
					meshData->mVertices[vertId].boneIds[1] = model.skins[0].joints[jointVec[1]];
					meshData->mVertices[vertId].boneIds[2] = model.skins[0].joints[jointVec[2]];
					meshData->mVertices[vertId].boneIds[3] = model.skins[0].joints[jointVec[3]];
				}
				else
				{
					// Default to no bone influence
					for (int j = 0; j < 4; ++j)
					{
						meshData->mVertices[vertId].boneIds[j] = -1;
					}
				}

				// Weights (optional)
				if (weights)
				{
					auto weightVec = glm::make_vec4(&weights[vertId * VEC4_STEP]);
					meshData->mVertices[vertId].weights[0] = weightVec[0];
					meshData->mVertices[vertId].weights[1] = weightVec[1];
					meshData->mVertices[vertId].weights[2] = weightVec[2];
					meshData->mVertices[vertId].weights[3] = weightVec[3];
				}
				else
				{
					// Default to no bone weight
					for (int j = 0; j < 4; ++j)
					{
						meshData->mVertices[vertId].weights[j] = 0.0f;
					}
				}
			}

			meshData->mMinBounds = minBounds;
			meshData->mMaxBounds = maxBounds;

			// Indices
			if (primitive.indices != -1)
			{
				int indicesCount = 0;
				const auto indices = gltfHelpers::GetgltfBuffer<const unsigned short
					*>(model, primitive.indices, indicesCount);

				meshData->mIndices.resize(indicesCount);
				for (int index = 0; index < indicesCount; ++index)
				{
					meshData->mIndices[index] = indices[index];
				}
			}

			// Handle textures
			int materialIndex = primitive.material;
			if (materialIndex >= 0 && materialIndex < model.materials.size())
			{
				const auto& material = model.materials[materialIndex];
				const auto& pbr = material.pbrMetallicRoughness;

				int texIndex = pbr.baseColorTexture.index;
				if (texIndex >= 0 && texIndex < model.textures.size())
				{
					const auto& texture = model.textures[texIndex];
					int imageIndex = texture.source;

					if (imageIndex >= 0 && imageIndex < model.images.size())
					{
						const auto& image = model.images[imageIndex];
						meshData->mTextureId = _LoadNewTexture(renderSystem, image);
					}
				}
			}
		}

		newModel.AddMesh(meshData);
	}
}

void ResourceManager::_ProcessJointData(const tinygltf::Model& model, Model& newModel) const
{
	for (const tinygltf::Skin& skin : model.skins)
	{
		// Get a pointer to the raw matrix data
		int matrixByteOffset = 0;
		const auto matrixData = gltfHelpers::GetgltfBuffer<const float*>(
			model, skin.inverseBindMatrices, matrixByteOffset);

		newModel.SetRootNodeIndex(skin.joints[0]);

		// Loop over each bone node (joint) in the skin
		for (size_t i = 0; i < skin.joints.size(); ++i)
		{
			int nodeIndex = skin.joints[i];
			const tinygltf::Node& node = model.nodes[nodeIndex];

			auto joint = new JointNode();
			joint->mId = node.name;
			joint->mTransformation = gltfHelpers::GetNodeTransform(node);
			joint->mInverseBindMatrix = glm::make_mat4(&matrixData[i * 16]);

			newModel.AddBoneNameToIndex(node.name, nodeIndex);

			// Add children if the node has any
			for (int child : node.children)
			{
				joint->mChildren.push_back(child);
			}

			// Store the bone info in the map (using node index as the key)
			newModel.AddJointNode(nodeIndex, joint);
		}
	}
}

void ResourceManager::_ProcessAnimations(const tinygltf::Model& model, Model& newModel) const
{
	for (const auto& animation : model.animations)
	{
		auto modelAnim = new Animation();
		modelAnim->mName = animation.name;
		for (const auto& channel : animation.channels)
		{
			const auto& sampler = animation.samplers[channel.sampler];

			bool newNode = false;

			AnimNode* animNode = nullptr;
			for (const auto node : modelAnim->mAnimNodes)
			{
				if (node->mNodeIndex == channel.target_node)
				{
					animNode = node;
					break;
				}
			}

			if (animNode == nullptr)
			{
				animNode = new AnimNode();
				animNode->mNodeIndex = channel.target_node;
				newNode = true;
			}

			// Get keyframe times (timestamps)
			int keyframeCount = 0;
			const auto keyframeTimes = gltfHelpers::GetgltfBuffer<const float*>(model, sampler.input, keyframeCount);

			if (channel.target_path == "translation")
			{
				const auto positions = gltfHelpers::GetgltfBuffer<const float*>(model, sampler.output);

				for (int i = 0; i < keyframeCount; ++i)
				{
					KeyframeVec keyframe;
					keyframe.mData = {
						positions[VEC3_STEP * i], positions[VEC3_STEP * i + 1], positions[VEC3_STEP * i + 2]
					};
					keyframe.mTimeStamp = keyframeTimes[i];
					animNode->mPositions.push_back(keyframe);
				}
			}
			else if (channel.target_path == "rotation")
			{
				const auto rotations = gltfHelpers::GetgltfBuffer<const float*>(model, sampler.output);

				for (int i = 0; i < keyframeCount; ++i)
				{
					KeyframeQuat keyframe;
					keyframe.mData = {
						glm::normalize(glm::quat(
							rotations[VEC4_STEP * i + 3], rotations[VEC4_STEP * i], rotations[VEC4_STEP * i + 1],
							rotations[VEC4_STEP * i + 2]))
					};
					keyframe.mTimeStamp = keyframeTimes[i];
					animNode->mRotations.push_back(keyframe);
				}
			}
			else if (channel.target_path == "scale")
			{
				const auto scales = gltfHelpers::GetgltfBuffer<const float*>(model, sampler.output);

				for (int i = 0; i < keyframeCount; ++i)
				{
					KeyframeVec keyframe;
					keyframe.mData = {scales[VEC3_STEP * i], scales[VEC3_STEP * i + 1], scales[VEC3_STEP * i + 2]};
					keyframe.mTimeStamp = keyframeTimes[i];
					animNode->mScales.push_back(keyframe);
				}
			}

			// Update the animation's start and end frame times
			modelAnim->mStartFrameTime = keyframeTimes[0];
			for (int i = 0; i < keyframeCount; ++i)
			{
				modelAnim->mStartFrameTime = std::min(modelAnim->mStartFrameTime, keyframeTimes[i]);
				modelAnim->mEndFrameTime = std::max(modelAnim->mEndFrameTime, keyframeTimes[i]);
			}

			if (newNode)
			{
				modelAnim->mAnimNodes.push_back(std::move(animNode));
			}
		}

		newModel.AddAnimation(modelAnim);
	}
}
