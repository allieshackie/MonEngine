#pragma once
#include <glm/mat4x4.hpp>

struct Vertex
{
	glm::vec3 position = {0, 0, 0};
	glm::vec4 color = {0, 0, 0, 0};
	glm::vec2 texCoord = {0, 0};
};

struct TexturedVertex
{
	glm::vec3 position = {0, 0, 0};
	glm::vec3 normal = {0, 0, 0};
	glm::vec2 texCoord = {0, 0};
};

struct DebugVertex
{
	glm::vec3 pos = {0, 0, 0};
	glm::vec4 color = {0, 0, 0, 0};
};
