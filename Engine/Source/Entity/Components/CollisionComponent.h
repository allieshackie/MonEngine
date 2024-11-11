#pragma once
#include <glm/vec3.hpp>
#include "Util/SerialUtil.h"
#include <BulletDynamics/Dynamics/btRigidBody.h>

struct CollisionComponent
{
	ColliderShapes mColliderShape = ColliderShapes::BOX;
	glm::vec3 mSize = {0, 0, 0};
	bool mIsDynamic = false;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("collider_shape", mColliderShape),
		        cereal::make_nvp("size", mSize));
	}

	// Runtime
	int mColliderIndex = -1;
	btRigidBody* mRigidBody = nullptr;
	bool mInitialized = false;
	float mSimulateTimeout = 5.0f;
};
