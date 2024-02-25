#pragma once
#include <glm/vec3.hpp>

struct PhysicsComponent
{
	float mMass = 1.0f;
	float mMaxSpeed = 10.0f;
	float mAccelerationRate = 10.0f;
	float mFriction = 1.0f;

	bool mIsDynamic = false; 

	glm::vec3 mAcceleration = {0, 0, 0};
	glm::vec3 mVelocity = {0, 0, 0};
};
