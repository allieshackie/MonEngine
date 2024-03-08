#pragma once
#include "Vertex.h"
#include "LLGL/RenderSystem.h"
#include "LLGL/Utils/ColorRGBA.h"

class Mesh
{
public:
	Mesh(const std::string& path);

	std::uint32_t GetFirstVertex() const { return mFirstVertex; }
	std::uint32_t GetNumVertices() const { return mNumVertices; }

	const std::vector<TexturedVertex>& GetVertices() const { return mVertices; }

private:
	void _LoadObjModel(const std::string& filename);

	std::uint32_t mFirstVertex = 0;
	std::uint32_t mNumVertices = 0;

	std::vector<TexturedVertex> mVertices;
};
