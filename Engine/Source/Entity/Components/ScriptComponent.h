#pragma once
#include <string>
#include "Util/SerialUtil.h"

struct ScriptComponent
{
	std::string mPath;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("path", mPath));
	}
};
