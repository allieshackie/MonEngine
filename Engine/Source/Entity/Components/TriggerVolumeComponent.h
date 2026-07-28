#pragma once
#include "Util/SerialUtil.h"

struct TriggerVolumeComponent
{
    glm::vec3 mSize;
    bool mEnabled = true;

	template <class Archive>
	void serialize(Archive& archive)
	{
		cereal::make_optional_nvp(archive, "enabled", mEnabled);
		archive(cereal::make_nvp("size", mSize));
	}
};