#pragma once
#include "Vertex.h"
#include "LLGL/RenderSystem.h"
#include "LLGL/Utils/ColorRGBA.h"

struct aiMesh;

class Mesh
{
public:
	Mesh(const std::string& path, std::string fileName);

	std::uint32_t GetNumIndices() const { return mNumIndices; }
	std::uint32_t GetNumVertices() const { return mNumVertices; }
	const std::string& GetId() const { return mId; }

	const std::vector<Vertex>& GetVertices() const { return mVertices; }
	const std::vector<uint32_t>& GetIndices() const { return mIndices; }

private:
	void _LoadObjModel(aiMesh* mesh);

	std::uint32_t mNumVertices = 0;
	std::uint32_t mNumIndices = 0;

	std::string mId;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;
};
