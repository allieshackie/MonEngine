#pragma once
#include "LLGL/RenderSystem.h"
#include "Vertex.h"

struct aiAnimation;
struct aiNodeAnim;
struct aiNode;
struct aiMesh;
struct aiScene;

class Shader;

struct BoneInfo
{
	int mId;
	glm::mat4 mOffset; // offset to convert the vertex from world space to bone space
};

struct MeshData
{
	std::uint32_t mNumVertices = 0;
	std::uint32_t mNumIndices = 0;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;

	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Buffer* mIndexBuffer = nullptr;
};

class Model
{
public:
	Model() = default;
	Model(const std::string& fullPath, std::string fileName);
	Model(const Model& other) = default;

	void InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader);

	const std::string& GetId() const { return mId; }
	const glm::mat4& GetRootInverseTransform() const { return mRootInverseTransform; }
	const std::vector<MeshData>& GetMeshes() const { return mMeshes; }
	const glm::mat4& GetBoneOffset(int boneIndex) const;

	aiAnimation* GetAnimation(const std::string& animationName);
	aiNode* GetRootNode() const { return mRootNode; }
	int GetBoneIndex(const std::string& boneName);

private:
	void _ProcessMesh(aiMesh* mesh);
	void _ProcessBoneWeights(aiMesh* mesh, std::vector<Vertex>& vertices);
	void _ProcessAnimations(const aiScene* scene);

	// Setup for animation
	void _AddAnimation(aiAnimation* animation);

	std::string mId;
	std::vector<MeshData> mMeshes;

	std::unordered_map<std::string, int> mBoneNameToIndex;
	std::vector<BoneInfo> mBoneInfos;
	int mBoneCount = 0;

	std::unordered_map<std::string, aiAnimation*> mAnimations;

	const aiScene* mScene = nullptr;
	aiNode* mRootNode = nullptr;
	glm::mat4 mRootInverseTransform = glm::mat4(1.0f);
};
