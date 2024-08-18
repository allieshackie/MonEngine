#pragma once
#include "LLGL/RenderSystem.h"
#include "Animation.h"
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
	Model(const aiScene* scene, std::string fileName);
	Model(const Model& other) = default;

	void InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader);

	const std::string& GetId() const { return mId; }
	std::vector<MeshData> GetMeshes() const { return mMeshes; }
	Animation& GetAnimation(const std::string& animationName);

private:
	void _ProcessMesh(aiMesh* mesh);
	void _ProcessBoneWeights(aiMesh* mesh, std::vector<Vertex>& vertices);
	void _ProcessAnimations(const aiScene* scene);

	// Setup for animation
	void _AddAnimation(const aiAnimation* animation, aiNode* rootNode);
	void _AddBoneAnim(Animation& anim, const aiNodeAnim* channel);

	std::string mId;
	std::vector<MeshData> mMeshes;

	std::unordered_map<std::string, BoneInfo> mBoneInfoMap;
	int mBoneCount = 0;

	std::unordered_map<std::string, Animation> mAnimations;
	Animation mEmptyAnimation;

	glm::mat4 mRootInverseTransform = glm::mat4(1.0f);
};
