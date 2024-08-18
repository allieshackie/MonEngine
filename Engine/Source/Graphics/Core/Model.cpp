#include <assimp/scene.h>           // Output data structure
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"
#include "Util/AssimpGLM.h"

#include "Model.h"

Model::Model(const aiScene* scene, std::string fileName) : mId(std::move(fileName))
{
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		_ProcessMesh(scene->mMeshes[i]);
	}

	mRootInverseTransform = AssimpGLM::ConvertMatrix(scene->mRootNode->mTransformation.Inverse());

	_ProcessAnimations(scene);
}

void Model::InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader)
{
	for (auto& mesh : mMeshes)
	{
		mesh.mVertexBuffer = renderSystem->CreateBuffer(
			LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mesh.mVertices.size() * sizeof(Vertex)),
			                       shader.GetVertexFormat()), mesh.mVertices.data());

		mesh.mIndexBuffer = renderSystem->CreateBuffer(
			LLGL::IndexBufferDesc(static_cast<std::uint32_t>(mesh.mIndices.size() * sizeof(uint32_t)),
			                      LLGL::Format::R32UInt),
			mesh.mIndices.data());

		mesh.mNumIndices = static_cast<int>(mesh.mIndices.size());
	}
}

Animation& Model::GetAnimation(const std::string& animationName)
{
	auto it = mAnimations.find(animationName);
	if (it != mAnimations.end())
	{
		return it->second;
	}

	return mEmptyAnimation;
}

void Model::_ProcessMesh(aiMesh* mesh)
{
	MeshData meshData;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		vertex.position = AssimpGLM::ConvertVec(mesh->mVertices[i]);
		vertex.normal = AssimpGLM::ConvertVec(mesh->mNormals[i]);

		// UVs coordinates
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.texCoord = glm::vec2(0.0f, 0.0f);
		}

		for (int i = 0; i < 4; i++)
		{
			vertex.boneIds[i] = -1;
			vertex.weights[i] = 0.0f;
		}

		meshData.mVertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			meshData.mIndices.push_back(face.mIndices[j]);
		}
	}

	_ProcessBoneWeights(mesh, meshData.mVertices);

	mMeshes.push_back(meshData);
}

void Model::_ProcessBoneWeights(aiMesh* mesh, std::vector<Vertex>& vertices)
{
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		int boneId = -1;
		std::string boneName = mesh->mBones[i]->mName.C_Str();
		if (mBoneInfoMap.find(boneName) == mBoneInfoMap.end())
		{
			BoneInfo boneInfo = {mBoneCount, AssimpGLM::ConvertMatrix(mesh->mBones[i]->mOffsetMatrix)};
			mBoneInfoMap[boneName] = boneInfo;
			boneId = mBoneCount;
			mBoneCount++;
		}
		else
		{
			boneId = mBoneInfoMap[boneName].mId;
		}

		assert(boneId != -1);

		auto weights = mesh->mBones[i]->mWeights;
		int numWeights = mesh->mBones[i]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			// TODO: When dealing with multiple meshes in a single scene, might need to do vertexId + last vertexId of previous mesh
			auto& vertex = vertices[vertexId];
			//assert(vertexId <= )
			for (int j = 0; j < 4; ++j)
			{
				if (vertex.boneIds[j] < 0)
				{
					vertex.weights[j] = weight;
					vertex.boneIds[j] = boneId;
					break;
				}
			}
		}
	}
}

void Model::_ProcessAnimations(const aiScene* scene)
{
	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		_AddAnimation(scene->mAnimations[i], scene->mRootNode);
	}
}

void Model::_AddAnimation(const aiAnimation* animation, aiNode* rootNode)
{
	Animation anim;

	// Find the position of the '|'
	std::string name = animation->mName.C_Str();
	size_t pos = name.find('|');

	if (pos != std::string::npos)
	{
		name = name.substr(pos + 1);
	}

	anim.mDuration = animation->mDuration;
	anim.mTicksPerSecond = animation->mTicksPerSecond;
	anim.mRootNode = rootNode;

	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		_AddBoneAnim(anim, animation->mChannels[i]);
	}

	mAnimations[name] = anim;
}

void Model::_AddBoneAnim(Animation& anim, const aiNodeAnim* channel)
{
	BoneAnimNode node;

	node.mNumPositions = channel->mNumPositionKeys;
	for (int i = 0; i < node.mNumPositions; i++)
	{
		node.mPositions.push_back({
			AssimpGLM::ConvertVec(channel->mPositionKeys[i].mValue), channel->mPositionKeys[i].mTime
		});
	}

	node.mNumRotations = channel->mNumRotationKeys;
	for (int i = 0; i < node.mNumRotations; i++)
	{
		node.mRotations.push_back({
			AssimpGLM::ConvertQuat(channel->mRotationKeys[i].mValue), channel->mRotationKeys[i].mTime
		});
	}

	node.mNumScales = channel->mNumScalingKeys;
	for (int i = 0; i < node.mNumScales; i++)
	{
		node.mScales.push_back({
			AssimpGLM::ConvertVec(channel->mScalingKeys[i].mValue), channel->mScalingKeys[i].mTime
		});
	}

	anim.mBoneAnimNodes.push_back(node);
}
