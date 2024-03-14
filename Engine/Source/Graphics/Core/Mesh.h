#pragma once
#include "Vertex.h"
#include "LLGL/RenderSystem.h"
#include "LLGL/Utils/ColorRGBA.h"

class Mesh
{
public:
	Mesh(const std::string& path, std::string fileName);

	std::uint32_t GetFirstVertex() const { return mFirstVertex; }
	std::uint32_t GetNumVertices() const { return mNumVertices; }
	const std::string& GetId() const { return mId; }

	const std::vector<TexturedVertex>& GetVertices() const { return mVertices; }
	const std::vector<int>& GetIndices() const { return mIndices; }

private:
	void _LoadObjModel(const std::string& path);

	std::uint32_t mFirstVertex = 0;
	std::uint32_t mNumVertices = 0;

	std::string mId;

	std::vector<TexturedVertex> mVertices;
	std::vector<int> mIndices;
};
