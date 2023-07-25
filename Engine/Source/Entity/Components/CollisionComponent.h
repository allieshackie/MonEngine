#pragma once
#include <glm/vec3.hpp>

enum ColliderShapes
{
	Box,
	Sphere,
	Capsule,
};

struct CollisionComponent
{
	int mColliderShape = ColliderShapes::Box;
	glm::vec3 mSize = {0, 0, 0};
};
