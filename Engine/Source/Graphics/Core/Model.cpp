#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "LLGL/RenderSystem.h"
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Node.h"
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"
#include "Util/gltfHelpers.h"

#include "Model.h"

Model::Model(int id, size_t nodesSize): mId(id), mNumNodes(nodesSize)
{
	for (int i = 0; i < nodesSize; i++)
	{
		mNodes.push_back(new Node());
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

JointData* Model::GetJointDataAt(int nodeIndex) const
{
	if (const auto it = mJointData.find(nodeIndex); it != mJointData.end())
	{
		return it->second;
	}

	return nullptr;
}

const Animation* Model::GetAnimation(int index) const
{
	if (index > mAnimations.size())
	{
		return nullptr;
	}

	return mAnimations[index];
}

glm::vec3 Model::CalculateModelScaling(const glm::vec3& targetSize) const
{
	glm::vec3 size = mMeshes[0]->mMaxBounds - mMeshes[0]->mMinBounds;
	// Scale uniformly to fit within targetSize (e.g., 1x1x1 meters)
	return targetSize / size;
}

Node* Model::GetNodeAt(int nodeIndex) const
{
	if (nodeIndex > mNodes.size())
	{
		return nullptr;
	}

	return mNodes[nodeIndex];
}

MeshData* Model::GetMeshAt(int nodeIndex) const
{
	if (nodeIndex > mMeshes.size())
	{
		return nullptr;
	}

	return mMeshes[nodeIndex];
}
