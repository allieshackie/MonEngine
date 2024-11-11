#pragma once
#include <glm/vec4.hpp>
#include "Util/SerialUtil.h"

enum class LightType
{
	POINT = 0,
	DIRECTIONAL
};

struct LightComponent
{
	glm::vec4 mAmbient = {0, 0, 0, 0};
	glm::vec4 mDiffuse = {0, 0, 0, 0};
	glm::vec4 mSpecular = {0, 0, 0, 0};
	float mIntensity = 1.0f;
	LightType mLightType = LightType::POINT;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("ambient", mAmbient),
		        cereal::make_nvp("diffuse", mDiffuse),
		        cereal::make_nvp("specular", mSpecular),
		        cereal::make_nvp("intensity", mIntensity),
		        cereal::make_nvp("light_type", mLightType));
	}
};
