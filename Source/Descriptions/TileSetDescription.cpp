#include "nlohmann/json.hpp"

#include "TileSetDescription.h"

using json = nlohmann::json;

const std::string& TileSetDescription::getTexturePath() const
{
	return mTexturePath;
}

int TileSetDescription::GetRows() const
{
	return mTilesetRows;
}

int TileSetDescription::GetColumns() const
{
	return mTilesetColumns;
}

int TileSetDescription::GetTotalTiles() const
{
	return mTilesetRows * mTilesetColumns;
}

// x,y top left position
// z,w width and height
glm::vec4 TileSetDescription::GetClipForTile(int index) const
{
	const float clipAcross = mTextureSize.x / mTilesetColumns;
	const float clipDown = mTextureSize.y / mTilesetRows;
	const float calculateDown = std::floor(index / mTilesetColumns);
	const float calculateAcross = index % mTilesetColumns;
	return {clipAcross * calculateAcross, clipDown * calculateDown, clipAcross, clipDown};
}

void TileSetDescription::SetRows(int rows)
{
	mTilesetRows = rows;
}

void TileSetDescription::setColumns(int columns)
{
	mTilesetColumns = columns;
}

void TileSetDescription::UpdateSize(int rows, int columns)
{
	/*
	 *
	if (mTileSetFilePath != JSON_PATH)
	{
		mTilesetRows = rows;
		mTilesetColumns = columns;

		mJson[SIZE_STRING][0] = rows;
		mJson[SIZE_STRING][1] = columns;

		std::ofstream ofs(mTileSetFilePath.c_str(), std::ios::out | std::ios::trunc);
		ofs << std::setw(4) << mJson << std::endl;
	}
	 */
}
