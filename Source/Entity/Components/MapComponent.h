#pragma once
#include <string>

struct MapComponent
{
	int mRows = 0;
	int mColumns = 0;
	int mTileSize = 0;
	std::string mMapDataFile;
};
