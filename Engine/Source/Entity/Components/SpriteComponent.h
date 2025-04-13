#pragma once
#include <string>
#include "Util/SerialUtil.h"

struct SpriteComponent
{
	std::string mTexturePath;
	int mRows = 0;
	int mColumns = 0;

	template <class Archive>
	void save(Archive& archive) const
	{
		archive(cereal::make_nvp("resource_path", mTexturePath),
		        cereal::make_nvp("rows", mRows),
		        cereal::make_nvp("columns", mColumns));
	}

	template <class Archive>
	void load(Archive& archive)
	{
		archive(cereal::make_nvp("resource_path", mTexturePath));

		cereal::make_optional_nvp(archive, "rows", mRows);
		cereal::make_optional_nvp(archive, "columns", mColumns);
	}
};
