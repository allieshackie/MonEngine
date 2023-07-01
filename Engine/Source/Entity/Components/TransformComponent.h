#pragma once
#include <glm/vec3.hpp>

struct TransformComponent
{
	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mSize = {0, 0, 0};
	glm::vec3 mRotation = {0, 0, 0};
};
