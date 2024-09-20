#pragma once
#include "assimp/Importer.hpp"
#include "Animation.h"
#include "LLGL/RenderSystem.h"
#include "Vertex.h"

struct aiAnimation;
struct aiMesh;
struct aiScene;

class Shader;

struct BoneInfo
{
	// offset to convert the vertex from world space to bone space
	glm::mat4 mOffset = glm::mat4(1.0f);
	glm::mat4 mFinalTransform = glm::mat4(1.0f);
};

struct MeshData
{
	std::uint32_t mBaseVertex = 0;
	std::uint32_t mBaseIndex = 0;
	std::uint32_t mNumIndices = 0;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;

	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Buffer* mIndexBuffer = nullptr;
};

class Model
{
public:
	Model(const std::string& fullPath, std::string fileName);

	void InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader) const;

	const std::string& GetId() const { return mId; }
	const glm::mat4& GetRootInverseTransform() const { return mGlobalInverseTransform; }
	const std::vector<MeshData*>& GetMeshes() const { return mMeshes; }
	const glm::mat4& GetBoneOffset(int boneIndex) const;

	aiAnimation* GetAnimation(const std::string& animationName) const;
	BoneNode* GetRootNode() const { return mRootNode; }
	int GetBoneIndex(const std::string& boneName) const;
	int GetBoneCount() const { return static_cast<int>(mBoneNameToIndex.size()); }
	const std::vector<BoneInfo*>& GetBoneInfos() const { return mBoneInfos; }

	const std::unordered_map<std::string, int>& GetBoneNamesToIndex() const { return mBoneNameToIndex; }

private:
	void _ProcessMesh(unsigned int meshIndex, aiMesh* mesh);
	void _ProcessBoneWeights(aiMesh* mesh, std::vector<Vertex>& vertices);

	std::string mId;
	std::vector<MeshData*> mMeshes;

	std::unordered_map<std::string, int> mBoneNameToIndex;
	std::vector<BoneInfo*> mBoneInfos;
	std::vector<BoneNode*> mBones;

	BoneNode* mRootNode = nullptr;
	glm::mat4 mGlobalInverseTransform = glm::mat4(1.0f);

	Assimp::Importer* mImporter = nullptr;
	const aiScene* mScene = nullptr;
};
