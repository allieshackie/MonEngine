#pragma once
#include <string>
#include "Util/SerialUtil.h"

struct ScriptComponent
{
	std::string mPath;
	bool mIsTrigger = false;

	// runtime
	int mLuaTableRef = -1;

	template <class Archive>
	void serialize(Archive& archive)
	{
		cereal::make_optional_nvp(archive, "is_trigger", mIsTrigger);
		archive(cereal::make_nvp("path", mPath));
	}
};
