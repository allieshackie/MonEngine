#pragma once

#include "Vertex.h"
#include "Graphics/Animation/Animation.h"

namespace LLGL
{
	class Buffer;
}

struct Node
{
	std::string mName;

	glm::mat4 mTransform = glm::mat4(1.0f);

	int mMeshIndex = -1;
	int mJointIndex = -1;

	std::vector<int> mChildren;
};

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

struct JointData
{
	std::string mId;
	//glm::mat4 mWorldTransformation = glm::mat4(1.0f);

	// Convert bone from world space to bone space
	glm::mat4 mInverseBindMatrix = glm::mat4(1.0f);
};
