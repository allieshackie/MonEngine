#include "assimp/postprocess.h"
#include <assimp/scene.h>
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"
#include "Util/AssimpGLM.h"

#include "Model.h"

Model::Model(const std::string& fullPath, std::string fileName) : mId(std::move(fileName))
{
	mImporter = new Assimp::Importer();
	mScene = mImporter->ReadFile(fullPath,
	                             aiProcessPreset_TargetRealtime_Quality);
	// Check if there was errors with 
	if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode)
	{
		std::cout << mImporter->GetErrorString() << std::endl;
	}

	if (mScene->mNumAnimations > 0)
	{
		mRootNode = AssimpGLM::ConvertAiNode(mScene->mRootNode);
		mGlobalInverseTransform = AssimpGLM::ConvertMatrix(mScene->mRootNode->mTransformation.Inverse());
	}

	int totalVertices = 0;
	int totalIndices = 0;

	// CountVerticesAndIndices
	for (unsigned int i = 0; i < mScene->mNumMeshes; i++)
	{
		auto meshData = new MeshData();
		meshData->mNumIndices = mScene->mMeshes[i]->mNumFaces * 3;
		meshData->mBaseVertex = totalVertices;
		meshData->mBaseIndex = totalIndices;

		totalVertices += mScene->mMeshes[i]->mNumVertices;
		totalIndices += meshData->mNumIndices;

		mMeshes.push_back(meshData);
	}

	for (unsigned int i = 0; i < mScene->mNumMeshes; i++)
	{
		_ProcessMesh(i, mScene->mMeshes[i]);
	}
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

const glm::mat4& Model::GetBoneOffset(int boneIndex) const
{
	return mBoneInfos[boneIndex]->mOffset;
}

aiAnimation* Model::GetAnimation(const std::string& animationName) const
{
	for (int i = 0; i < mScene->mNumAnimations; i++)
	{
		if (strcmp(mScene->mAnimations[i]->mName.C_Str(), animationName.c_str()) == 0)
		{
			return mScene->mAnimations[i];
		}
	}

	return nullptr;
}

int Model::GetBoneIndex(const std::string& boneName) const
{
	if (auto it = mBoneNameToIndex.find(boneName); it != mBoneNameToIndex.end())
	{
		return it->second;
	}

	return -1;
}

void Model::_ProcessMesh(unsigned int meshIndex, aiMesh* mesh)
{
	auto& meshData = mMeshes[meshIndex];
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

		meshData->mVertices.push_back(vertex);
	}

	// LoadMeshBones
	_ProcessBoneWeights(mesh, meshData->mVertices);

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			meshData->mIndices.push_back(face.mIndices[j]);
		}
	}
}

void Model::_ProcessBoneWeights(aiMesh* mesh, std::vector<Vertex>& vertices)
{
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		// GetBoneId
		int boneId = 0;
		std::string boneName = mesh->mBones[i]->mName.C_Str();
		if (mBoneNameToIndex.find(boneName) == mBoneNameToIndex.end())
		{
			boneId = static_cast<int>(mBoneNameToIndex.size());
			mBoneNameToIndex[boneName] = boneId;
		}
		else
		{
			boneId = mBoneNameToIndex[boneName];
		}

		if (boneId == mBoneInfos.size())
		{
			auto info = new BoneInfo();
			info->mOffset = AssimpGLM::ConvertMatrix(mesh->mBones[i]->mOffsetMatrix);
			mBoneInfos.push_back(std::move(info));
		}

		auto weights = mesh->mBones[i]->mWeights;

		for (unsigned int weightIndex = 0; weightIndex < mesh->mBones[i]->mNumWeights; ++weightIndex)
		{
			float weight = weights[weightIndex].mWeight;
			auto& vertex = vertices[weights[weightIndex].mVertexId];
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
