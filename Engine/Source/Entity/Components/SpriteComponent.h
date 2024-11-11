#pragma once
#include <string>
#include "Util/SerialUtil.h"

struct SpriteComponent
{
	std::string mTexturePath;
	int mRows = 0;
	int mColumns = 0;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("resource_path", mTexturePath),
		        cereal::make_nvp("rows", mRows),
		        cereal::make_nvp("columns", mColumns));
	}
};
