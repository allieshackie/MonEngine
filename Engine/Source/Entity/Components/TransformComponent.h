#pragma once
#include <glm/vec3.hpp>
#include <bullet/LinearMath/btQuaternion.h>
#include "Util/SerialUtil.h"

struct TransformComponent
{
	glm::vec3 mPosition = glm::vec3(0.0f);
	glm::vec3 mSize = glm::vec3(1.0f);
	glm::quat mRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	glm::quat mQueuedRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("position", mPosition),
		        cereal::make_nvp("size", mSize));
	}

	glm::vec3 GetEulerAngles() const {
		return glm::degrees(glm::eulerAngles(mRotation));
	}

	void SetEulerAngles(const glm::vec3& degrees) {
		mRotation = glm::quat(glm::radians(degrees));
	}
	
	btQuaternion ToBulletQuat() const { return btQuaternion(mRotation.x, mRotation.y, mRotation.z, mRotation.w); }

	glm::mat4 GetMatrix(const glm::vec3& modelScale = glm::vec3(1.0f)) const {
		return glm::translate(glm::mat4(1.0f), mPosition)
			* glm::mat4_cast(mRotation)
			* glm::scale(glm::mat4(1.0f), modelScale);
	}
};
