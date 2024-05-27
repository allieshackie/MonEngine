#pragma once

#include "glm/vec3.hpp"

struct PhysicsComponent
{
	float mMass = 1.0f;
	float mFriction = 1.0f;

	glm::vec3 mVelocity = {0, 0, 0};
};
