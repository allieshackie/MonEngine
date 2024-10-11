#pragma once
#include <tiny_gltf.h>

#include "LLGL/RenderSystem.h"
#include "Animation.h"
#include "Vertex.h"

#define VEC2_STEP 2
#define VEC3_STEP 3
#define VEC4_STEP 4

class Shader;

struct MeshData
{
	std::string mName;
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
	int GetRootNodeIndex() const { return mRootNodeIndex; }
	const std::vector<MeshData*>& GetMeshes() const { return mMeshes; }

	JointNode& GetJointNodeAt(int nodeIndex);
	size_t GetNumJoints() const { return mBoneNameToIndex.size(); }

	const Animation* GetAnimation(const std::string& name) const;

	// TODO: For Debug GUI
	const std::unordered_map<std::string, int>& GetBoneNamesToIndex() const { return mBoneNameToIndex; }

private:
	void _ProcessMeshes(tinygltf::Model& model);
	void _ProcessJointData(const tinygltf::Model& model);
	void _ProcessAnimations(const tinygltf::Model& model);

	std::string mId;
	std::vector<MeshData*> mMeshes;

	std::unordered_map<std::string, int> mBoneNameToIndex;
	std::unordered_map<std::string, Animation*> mAnimations;

	std::unordered_map<int, JointNode> mJointNodes;
	int mRootNodeIndex = 0;
};
