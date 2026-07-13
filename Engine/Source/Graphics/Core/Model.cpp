#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "LLGL/RenderSystem.h"
#include "LLGL/Utils/Utility.h"

#include "Graphics/RenderSystem.h"
#include "Shader.h"
#include "Vertex.h"
#include "Util/gltfHelpers.h"

#include "Model.h"

Model::Model(int id, size_t nodesSize): mId(id), mNumNodes(nodesSize)
{
	for (int i = 0; i < nodesSize; i++)
	{
		mNodes.push_back(std::make_unique<MeshNode>());
	}
}

void Model::InitializeBuffers(const RenderSystem& renderSystem) const
{
	for (auto& mesh : mMeshes)
	{
		mesh->mVertexBuffer = renderSystem.GetSystem()->CreateBuffer(
			LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mesh->mVertices.size() * sizeof(Vertex)),
			                       renderSystem.GetMeshShader().GetVertexFormat()), mesh->mVertices.data());

		mesh->mIndexBuffer = renderSystem.GetSystem()->CreateBuffer(
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
		return it->second.get();
	}

	return nullptr;
}

const Animation* Model::GetAnimation(int index) const
{
	if (index > mAnimations.size())
	{
		return nullptr;
	}

	return mAnimations[index].get();
}

glm::vec3 Model::CalculateModelScaling(const glm::vec3& targetSize) const
{
	glm::vec3 modelSize = mMaxBounds - mMinBounds;

	glm::vec3 axisScales = targetSize / modelSize;
	float uniformScale = glm::min(axisScales.x, glm::min(axisScales.y, axisScales.z));

	return glm::vec3(uniformScale);
}

// Returns actual world-space size after applying target size
glm::vec3 Model::CalculateWorldBounds(const glm::vec3& targetSize) const
{
	glm::vec3 globalMin = glm::vec3(FLT_MAX);
	glm::vec3 globalMax = glm::vec3(-FLT_MAX);

	for (const auto& mesh : mMeshes)
	{
		globalMin = glm::min(globalMin, mesh->mMinBounds);
		globalMax = glm::max(globalMax, mesh->mMaxBounds);
	}

	glm::vec3 modelSize = globalMax - globalMin;
	float modelHeight = modelSize.y;
	float uniformScale = targetSize.y / modelHeight;  // height-driven, stays proportional
	return modelSize * uniformScale;
}

MeshNode* Model::GetNodeAt(int nodeIndex) const
{
	if (nodeIndex > mNodes.size())
	{
		return nullptr;
	}

	return mNodes[nodeIndex].get();
}

MeshData* Model::GetMeshAt(int nodeIndex) const
{
	if (nodeIndex > mMeshes.size())
	{
		return nullptr;
	}

	return mMeshes[nodeIndex].get();
}
