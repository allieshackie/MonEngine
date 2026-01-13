#pragma once 
#include <glm/mat4x4.hpp>
#include <LLGL/LLGL.h>
#include "Node.h"

class Shader;

class TerrainMesh 
{
public:
	TerrainMesh(const std::vector<std::vector<float>>& heightmap, float heightScale = 10.0f, float gridSpacing = 1.0f);

	void InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader);

	bool Initialized() const { return mMeshData.mVertexBuffer != nullptr; }

private:
	void _Generate(const std::vector<std::vector<float>>& heightmap, float heightScale = 10.0f, float gridSpacing = 1.0f);

	MeshData mMeshData;
};