#pragma once
#include <glm/mat4x4.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
};

struct TexturedVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct Model
{
	std::uint32_t numVertices = 0;
	std::uint32_t firstVertex = 0;
};

struct DebugVertex
{
	glm::vec3 pos;
	glm::vec4 color;
};
