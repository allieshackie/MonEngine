#pragma once
#include "Vertex.h"
#include "LLGL/RenderSystem.h"

struct aiMesh;

class Shader;

struct MeshData
{
	std::uint32_t mNumVertices = 0;
	std::uint32_t mNumIndices = 0;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;

	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Buffer* mIndexBuffer = nullptr;
};

class Model
{
public:
	Model() = default;
	Model(const std::string& path, std::string fileName);
	Model(const Model& other) = default;

	const std::string& GetId() const { return mId; }

	std::vector<MeshData> GetMeshes() const { return mMeshes; }

	void InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader);

private:
	void _LoadObjModel(aiMesh* mesh);

	std::string mId;

	std::vector<MeshData> mMeshes;
};
