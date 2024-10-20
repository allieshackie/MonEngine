#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"
#include "Util/gltfHelpers.h"

#include "Model.h"

Model::Model(const std::string& fullPath, std::string fileName) : mId(std::move(fileName))
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

	_ProcessMeshes(model);
	_ProcessJointData(model);
	_ProcessAnimations(model);

	mNumNodes = model.nodes.size();
}

void Model::InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader) const
{
	for (const auto mesh : mMeshes)
	{
		mesh->mVertexBuffer = renderSystem->CreateBuffer(
			LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mesh->mVertices.size() * sizeof(Vertex)),
			                       shader.GetVertexFormat()), mesh->mVertices.data());

		mesh->mIndexBuffer = renderSystem->CreateBuffer(
			LLGL::IndexBufferDesc(static_cast<std::uint32_t>(mesh->mIndices.size() * sizeof(uint32_t)),
			                      LLGL::Format::R32UInt),
			mesh->mIndices.data());

		mesh->mNumIndices = static_cast<int>(mesh->mIndices.size());
	}
}

JointNode* Model::GetJointNodeAt(int nodeIndex) const
{
	if (const auto it = mJointNodes.find(nodeIndex); it != mJointNodes.end())
	{
		return it->second;
	}

	return nullptr;
}

const Animation* Model::GetAnimation(AnimationStates state) const
{
	if (const auto it = mAnimations.find(state); it != mAnimations.end())
	{
		return it->second;
	}

	return nullptr;
}

void Model::_ProcessMeshes(tinygltf::Model& model)
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

			meshData->mVertices.resize(posCount);
			for (int vertId = 0; vertId < posCount; vertId++)
			{
				// Position
				meshData->mVertices[vertId].position = glm::make_vec3(&positions[vertId * VEC3_STEP]);

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
		}

		mMeshes.push_back(std::move(meshData));
	}
}

void Model::_ProcessJointData(const tinygltf::Model& model)
{
	for (const tinygltf::Skin& skin : model.skins)
	{
		// Get a pointer to the raw matrix data
		int matrixByteOffset = 0;
		const auto matrixData = gltfHelpers::GetgltfBuffer<const float*>(
			model, skin.inverseBindMatrices, matrixByteOffset);

		mRootNodeIndex = skin.joints[0];

		// Loop over each bone node (joint) in the skin
		for (size_t i = 0; i < skin.joints.size(); ++i)
		{
			int nodeIndex = skin.joints[i];
			const tinygltf::Node& node = model.nodes[nodeIndex];

			auto joint = new JointNode();
			joint->mId = node.name;
			joint->mTransformation = gltfHelpers::GetNodeTransform(node);
			joint->mInverseBindMatrix = glm::make_mat4(&matrixData[i * 16]);

			mBoneNameToIndex[node.name] = nodeIndex;

			// Add children if the node has any
			for (int child : node.children)
			{
				joint->mChildren.push_back(child);
			}

			// Store the bone info in the map (using node index as the key)
			mJointNodes[nodeIndex] = joint;
		}
	}
}

void Model::_ProcessAnimations(const tinygltf::Model& model)
{
	for (const auto& animation : model.animations)
	{
		auto modelAnim = new Animation();
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

		if (const auto it = ANIM_NAMES.find(animation.name); it != ANIM_NAMES.end())
		{
			mAnimations[it->second] = std::move(modelAnim);
		}
	}
}
