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
struct Node;

class Model
{
public:
	Model(int id, size_t nodesSize);

	void InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader) const;

	int GetId() const { return mId; }
	int GetRootNodeIndex() const { return mRootNodeIndex; }
	const std::vector<MeshData*>& GetMeshes() const { return mMeshes; }
	const std::vector<Node*>& GetNodes() const { return mNodes; }

	JointData* GetJointDataAt(int nodeIndex) const;
	size_t GetNumJoints() const { return mNumNodes; }

	const Animation* GetAnimation(int index) const;

	glm::vec3 CalculateModelScaling(const glm::vec3& targetSize) const;

	// TODO: For Debug GUI
	const std::unordered_map<std::string, int>& GetBoneNamesToIndex() const { return mBoneNameToIndex; }

	// Setup
	void AddMesh(MeshData* mesh) { mMeshes.push_back(mesh); }
	void SetRootNodeIndex(int index) { mRootNodeIndex = index; }
	void AddBoneNameToIndex(const std::string& name, int index) { mBoneNameToIndex[name] = index; }
	void AddJointData(int index, JointData* node) { mJointData[index] = node; }
	void AddAnimation(Animation* modelAnim) { mAnimations.push_back(modelAnim); }

	// Setup helpers
	Node* GetNodeAt(int nodeIndex) const;
	int GetMeshIndex(const std::string& name);
	int GetJointIndex(const std::string& name);

private:
	int mId;
	std::vector<Node*> mNodes;
	std::vector<MeshData*> mMeshes;

	std::unordered_map<std::string, int> mBoneNameToIndex;
	std::vector<Animation*> mAnimations;

	std::unordered_map<int, JointData*> mJointData;
	int mRootNodeIndex = 0;
	size_t mNumNodes = 0;
};
