#include "LLGL/RenderSystem.h"
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Shader.h"

#include "TerrainMesh.h"

TerrainMesh::TerrainMesh(const std::vector<std::vector<float>>& heightmap, float heightScale, float gridSpacing)
{
}

void TerrainMesh::InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader)
{
	mMeshData.mVertexBuffer = renderSystem->CreateBuffer(
		LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mMeshData.mVertices.size() * sizeof(Vertex)),
			shader.GetVertexFormat()), mMeshData.mVertices.data());

	mMeshData.mIndexBuffer = renderSystem->CreateBuffer(
		LLGL::IndexBufferDesc(static_cast<std::uint32_t>(mMeshData.mIndices.size() * sizeof(uint32_t)),
			LLGL::Format::R32UInt),
		mMeshData.mIndices.data());

	mMeshData.mNumIndices = static_cast<int>(mMeshData.mIndices.size());
}

void TerrainMesh::_Generate(const std::vector<std::vector<float>>& heightmap, float heightScale, float gridSpacing)
{
	int width = heightmap.size();
	int depth = heightmap[0].size();

	for (int j = 0; j < depth; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Vertex vert; 
			vert.position = { i * gridSpacing, heightmap[i][j] * heightScale, j * gridSpacing };
			vert.texCoord = { static_cast<float>(i) / (width - 1), static_cast<float>(j) / (depth - 1) };
			mMeshData.mVertices.push_back(vert);
		}
	}

	for (int j = 0; j < depth - 1; j++)
	{
		for (int i = 0; i < width - 1; i++)
		{
			int topLeft = j * width + i;
			int bottomLeft = (j + 1) * width + i;
			int topRight = topLeft + 1;
			int bottomRight = bottomLeft + 1;

			mMeshData.mIndices.emplace_back(topLeft);
			mMeshData.mIndices.emplace_back(bottomLeft);
			mMeshData.mIndices.emplace_back(topRight);
			mMeshData.mIndices.emplace_back(topRight);
			mMeshData.mIndices.emplace_back(bottomLeft);
			mMeshData.mIndices.emplace_back(bottomRight);
		}
	}

	// Calculate Normals
	for (int i = 0; i < mMeshData.mIndices.size(); i += 3)
	{
		const glm::vec3& v0 = mMeshData.mVertices[mMeshData.mIndices[i]].position;
		const glm::vec3& v1 = mMeshData.mVertices[mMeshData.mIndices[i+1]].position;
		const glm::vec3& v2 = mMeshData.mVertices[mMeshData.mIndices[i+2]].position;

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		mMeshData.mVertices[mMeshData.mIndices[i]].normal += normal;
		mMeshData.mVertices[mMeshData.mIndices[i+1]].normal += normal;
		mMeshData.mVertices[mMeshData.mIndices[i+2]].normal += normal;
	}

	// Pass to normalize
	for (auto& vertex : mMeshData.mVertices)
	{
		vertex.normal = glm::normalize(vertex.normal);
	}
}
