#pragma once

#include "Graphics/Animation/Animation.h"
#include "Vertex.h"

#define VEC2_STEP 2
#define VEC3_STEP 3
#define VEC4_STEP 4

class Shader;
class RenderSystem;

struct JointData;
struct MeshData;
struct MeshNode;

class Model
{
public:
	Model(int id, size_t nodesSize);

	void InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader) const;

	int GetId() const { return mId; }
	int GetRootJointIndex() const { return mRootJointIndex; }
	int GetRootSceneIndex() const { return mRootSceneIndex; }
	const std::vector<MeshData*>& GetMeshes() const { return mMeshes; }
	const std::vector<MeshNode*>& GetNodes() const { return mNodes; }

	JointData* GetJointDataAt(int nodeIndex) const;
	size_t GetNumJoints() const { return mNumNodes; }

	const Animation* GetAnimation(int index) const;
	const std::vector<Animation*>& GetAllAnimations() const { return mAnimations; }

	glm::vec3 CalculateModelScaling(const glm::vec3& targetSize) const;

	const std::unordered_map<std::string, int>& GetBoneNamesToIndex() const { return mBoneNameToIndex; }

	// Setup
	void AddMesh(MeshData* mesh) { mMeshes.push_back(mesh); }
	void SetRootJointIndex(int index) { mRootJointIndex = index; }
	void SetRootSceneIndex(int index) { mRootSceneIndex = index; }
	void AddBoneNameToIndex(const std::string& name, int index) { mBoneNameToIndex[name] = index; }
	void AddJointData(int index, JointData* node) { mJointData[index] = node; }
	void AddAnimation(Animation* modelAnim) { mAnimations.push_back(modelAnim); }

	// Setup helpers
	MeshNode* GetNodeAt(int nodeIndex) const;
	MeshData* GetMeshAt(int nodeIndex) const;

private:
	int mId;
	std::vector<MeshNode*> mNodes;
	std::vector<MeshData*> mMeshes;

	std::unordered_map<std::string, int> mBoneNameToIndex;
	std::vector<Animation*> mAnimations;

	std::unordered_map<int, JointData*> mJointData;
	int mRootJointIndex = 0;
	int mRootSceneIndex = 0;
	size_t mNumNodes = 0;
};
