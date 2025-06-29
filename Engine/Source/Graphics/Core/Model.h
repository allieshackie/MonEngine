#pragma once

#include "LLGL/RenderSystem.h"
#include "Graphics/Animation/Animation.h"
#include "Vertex.h"

#define VEC2_STEP 2
#define VEC3_STEP 3
#define VEC4_STEP 4

class Shader;

struct MeshData
{
	std::string mName;
	std::uint32_t mNumIndices = 0;
	int mTextureId = -1;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;

	glm::vec3 mMinBounds = {0, 0, 0};
	glm::vec3 mMaxBounds = {0, 0, 0};

	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Buffer* mIndexBuffer = nullptr;
};

class Model
{
public:
	Model(int id, size_t nodesSize);

	void InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader) const;

	int GetId() const { return mId; }
	int GetRootNodeIndex() const { return mRootNodeIndex; }
	const std::vector<MeshData*>& GetMeshes() const { return mMeshes; }

	JointNode* GetJointNodeAt(int nodeIndex) const;
	size_t GetNumJoints() const { return mNumNodes; }

	const Animation* GetAnimation(int index) const;

	glm::vec3 CalculateModelScaling(const glm::vec3& targetSize) const;

	// TODO: For Debug GUI
	const std::unordered_map<std::string, int>& GetBoneNamesToIndex() const { return mBoneNameToIndex; }

	// Setup
	void AddMesh(MeshData* mesh) { mMeshes.push_back(std::move(mesh)); }
	void SetRootNodeIndex(int index) { mRootNodeIndex = index; }
	void AddBoneNameToIndex(std::string name, int index) { mBoneNameToIndex[name] = index; }
	void AddJointNode(int index, JointNode* node) { mJointNodes[index] = node; }

	void AddAnimation(Animation* modelAnim) { mAnimations.push_back(std::move(modelAnim)); }

private:
	int mId;
	std::vector<MeshData*> mMeshes;

	std::unordered_map<std::string, int> mBoneNameToIndex;
	std::vector<Animation*> mAnimations;

	std::unordered_map<int, JointNode*> mJointNodes;
	int mRootNodeIndex = 0;
	size_t mNumNodes = 0;
};
