#pragma once
#include <glm/vec3.hpp>
#include "Util/SerialUtil.h"

enum MoveState
{
	None,
	Idle
};

struct TransformComponent
{
	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mSize = {0, 0, 0};
	glm::vec3 mRotation = {0, 0, 0};

	MoveState mState = MoveState::None;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("position", mPosition),
		        cereal::make_nvp("size", mSize),
		        cereal::make_nvp("rotation", mRotation));
	}
};
