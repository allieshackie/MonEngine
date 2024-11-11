#pragma once
#include "glm/vec3.hpp"
#include "Util/SerialUtil.h"

struct PhysicsComponent
{
	float mMass = 1.0f;
	float mFriction = 1.0f;

	glm::vec3 mVelocity = {0, 0, 0};

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("mass", mMass),
		        cereal::make_nvp("friction", mFriction));
	}
};
