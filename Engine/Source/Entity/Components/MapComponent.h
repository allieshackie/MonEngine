#pragma once
#include "Util/SerialUtil.h"

struct MapComponent
{
	std::string mId;

	int mRows = 0;
	int mColumns = 0;

	glm::vec2 mTextureSize = {0, 0};

	std::string mTexturePath;
	int mTextureMapRows = 0;
	int mTextureMapColumns = 0;

	std::string mDataPath;
	std::vector<int> mData;

	float mTileSize = 0.0f;

	// Runtime
	int mGeneratedTextureId = -1;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("id", mId),
		        cereal::make_nvp("rows", mRows),
		        cereal::make_nvp("columns", mColumns),
		        cereal::make_nvp("texture_size", mTextureSize),
		        cereal::make_nvp("texture_path", mTexturePath),
		        cereal::make_nvp("texture_map_rows", mTextureMapRows),
		        cereal::make_nvp("texture_map_columns", mTextureMapColumns),
		        cereal::make_nvp("data_path", mDataPath),
		        cereal::make_nvp("tile_size", mTileSize));
	}
};
