#pragma once

struct MapComponent
{
	std::string mId;
	//Set once texture has been generated
	int mGeneratedTextureId = -1;

	int mRows = 0;
	int mColumns = 0;

	std::array<uint32_t, 2> mTextureSize = {0, 0};

	std::string mTexturePath;
	int mTextureMapRows = 0;
	int mTextureMapColumns = 0;

	std::string mDataPath;
	std::vector<int> mData;

	float mTileSize = 0.0f;
};
