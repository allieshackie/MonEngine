#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <assimp/scene.h>
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"
#include "Util/AssimpGLM.h"

#include "Model.h"

Model::Model(const std::string& fullPath, std::string fileName) : mId(std::move(fileName))
{
	Assimp::Importer importer;
	mScene = importer.ReadFile(fullPath,
	                           aiProcessPreset_TargetRealtime_Quality);
	// Check if there was errors with 
	if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode)
	{
		std::cout << importer.GetErrorString() << std::endl;
	}
	for (unsigned int i = 0; i < mScene->mNumMeshes; i++)
	{
		_ProcessMesh(mScene->mMeshes[i]);
	}

	mRootNode = mScene->mRootNode;
	mRootInverseTransform = AssimpGLM::ConvertMatrix(mScene->mRootNode->mTransformation.Inverse());

	_ProcessAnimations(mScene);

	//TODO: aiReleaseImport when done with scene
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

const glm::mat4& Model::GetBoneOffset(int boneIndex) const
{
	return mBoneInfos[boneIndex].mOffset;
}

aiAnimation* Model::GetAnimation(const std::string& animationName)
{
	auto it = mAnimations.find(animationName);
	if (it != mAnimations.end())
	{
		return it->second;
	}

	return nullptr;
}

int Model::GetBoneIndex(const std::string& boneName)
{
	if (auto it = mBoneNameToIndex.find(boneName); it != mBoneNameToIndex.end())
	{
		return it->second;
	}

	return 0;
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
	mBoneInfos.resize(mesh->mNumBones);
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		int boneId = -1;
		std::string boneName = mesh->mBones[i]->mName.C_Str();
		if (mBoneNameToIndex.find(boneName) == mBoneNameToIndex.end())
		{
			boneId = mBoneCount;

			BoneInfo boneInfo = {boneId, AssimpGLM::ConvertMatrix(mesh->mBones[i]->mOffsetMatrix)};
			mBoneInfos[boneId] = boneInfo;
			mBoneNameToIndex[boneName] = boneId;
			mBoneCount++;
		}
		else
		{
			boneId = mBoneNameToIndex[boneName];
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
			for (int j = 0; j < 4; ++j)
			{
				if (vertex.weights[j] == 0.0f)
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
		_AddAnimation(scene->mAnimations[i]);
	}
}

void Model::_AddAnimation(aiAnimation* animation)
{
	// Find the position of the '|'
	std::string name = animation->mName.C_Str();
	size_t pos = name.find('|');

	if (pos != std::string::npos)
	{
		name = name.substr(pos + 1);
	}

	mAnimations[name] = animation;
}
