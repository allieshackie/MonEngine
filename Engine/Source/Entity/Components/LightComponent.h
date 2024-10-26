#pragma once
#include <glm/vec4.hpp>

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
};
