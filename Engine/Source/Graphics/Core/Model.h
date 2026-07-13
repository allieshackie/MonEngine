#pragma once

#include "Graphics/Animation/Animation.h"
#include "Node.h"
#include "Vertex.h"

#define VEC2_STEP 2
#define VEC3_STEP 3
#define VEC4_STEP 4

class Shader;
class RenderSystem;

class Model
{
public:
	Model(int id, size_t nodesSize);

	void InitializeBuffers(const RenderSystem& renderSystem) const;

	int GetId() const { return mId; }
	int GetRootJointIndex() const { return mRootJointIndex; }
	int GetRootSceneIndex() const { return mRootSceneIndex; }
	const std::vector<std::unique_ptr<MeshData>>& GetMeshes() const { return mMeshes; }
	const std::vector<std::unique_ptr<MeshNode>>& GetNodes() const { return mNodes; }

	JointData* GetJointDataAt(int nodeIndex) const;
	size_t GetNumJoints() const { return mNumNodes; }

	const Animation* GetAnimation(int index) const;
	const std::vector<std::unique_ptr<Animation>>& GetAllAnimations() const { return mAnimations; }

	glm::vec3 CalculateModelScaling(const glm::vec3& targetSize) const;
	glm::vec3 CalculateWorldBounds(const glm::vec3& targetSize) const;

	const std::unordered_map<std::string, int>& GetBoneNamesToIndex() const { return mBoneNameToIndex; }

	// Setup
	void AddMesh(std::unique_ptr<MeshData> mesh) { mMeshes.push_back(std::move(mesh)); }
	void SetRootJointIndex(int index) { mRootJointIndex = index; }
	void SetRootSceneIndex(int index) { mRootSceneIndex = index; }
	void AddBoneNameToIndex(const std::string& name, int index) { mBoneNameToIndex[name] = index; }
	void AddJointData(int index, std::unique_ptr<JointData> node) { mJointData[index] = std::move(node); }
	void AddAnimation(std::unique_ptr<Animation> modelAnim) { mAnimations.push_back(std::move(modelAnim)); }

	// Setup helpers
	MeshNode* GetNodeAt(int nodeIndex) const;
	MeshData* GetMeshAt(int nodeIndex) const;

	glm::vec3 GetMinBounds() const { return mMinBounds; }
	glm::vec3 GetMaxBounds() const { return mMaxBounds; }

	void SetMinBounds(glm::vec3 v) { mMinBounds = v; }
	void SetMaxBounds(glm::vec3 v) { mMaxBounds = v; }

private:
	int mId;
	std::vector<std::unique_ptr<MeshNode>> mNodes;
	std::vector<std::unique_ptr<MeshData>> mMeshes;

	std::unordered_map<std::string, int> mBoneNameToIndex;
	std::vector<std::unique_ptr<Animation>> mAnimations;

	std::unordered_map<int, std::unique_ptr<JointData>> mJointData;
	int mRootJointIndex = 0;
	int mRootSceneIndex = 0;
	size_t mNumNodes = 0;

	glm::vec3 mMinBounds = glm::vec3();
	glm::vec3 mMaxBounds = glm::vec3();
};
