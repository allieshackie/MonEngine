#pragma once
#include <glm/vec4.hpp>
#include "Util/SerialUtil.h"

struct InteractComponent
{
	bool mEnabled = true;

	template <class Archive>
	void serialize(Archive& archive)
	{
	}
};