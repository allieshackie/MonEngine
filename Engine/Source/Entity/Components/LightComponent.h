#pragma once
#include <glm/vec3.hpp>

enum class LightType
{
	POINT = 0,
	DIRECTIONAL
};

struct LightComponent
{
	glm::vec3 mColor = {0, 0, 0};
	LightType mLightType = LightType::POINT;
};
