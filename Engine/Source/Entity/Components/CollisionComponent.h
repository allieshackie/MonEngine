#pragma once
#include <glm/vec3.hpp>
#include <BulletDynamics/Dynamics/btRigidBody.h>

enum class ColliderShapes : int
{
	Box = 0,
	Sphere,
	Capsule,
};

struct CollisionComponent
{
	ColliderShapes mColliderShape = ColliderShapes::Box;
	glm::vec3 mSize = {0, 0, 0};
	int mColliderIndex = -1;
	bool mIsDynamic = false;

	btRigidBody* mRigidBody = nullptr;
	bool mInitialized = false;
	float mSimulateTimeout = 5.0f;
};
