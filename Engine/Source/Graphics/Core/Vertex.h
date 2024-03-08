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

struct DebugVertex
{
	glm::vec3 pos;
	glm::vec4 color;
};
