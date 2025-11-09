#pragma once
#include <glm/mat4x4.hpp>
#define MAX_BONE_INFLUENCE 4

struct Vertex
{
	glm::vec3 position = {0, 0, 0};
	glm::vec3 normal = {0, 0, 0};
	glm::vec2 texCoord = {0, 0};
	int boneIds[4] = {0, 0, 0, 0};
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

struct DebugVertex
{
	glm::vec3 pos = {0, 0, 0};
	glm::vec4 color = {0, 0, 0, 0};
};
