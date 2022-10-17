#pragma once
#include <glm/mat4x4.hpp>

struct Vertex
{
	glm::vec2 position;
	glm::vec2 texCoord;
};

struct DebugVertex
{
	glm::vec2 position;
	glm::vec3 color;
};

struct TexturedVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct TriangleMesh
{
	std::uint32_t firstVertex = 0;
	std::uint32_t numVertices = 0;
	glm::mat4 transform;
	LLGL::ColorRGBAf color;
};

struct Model
{
	std::uint32_t numVertices = 0;
	std::uint32_t firstVertex = 0;
};
