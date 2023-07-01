#pragma once
#include <glm/vec3.hpp>

struct MapComponent
{
	int mRows = 0;
	int mColumns = 0;
	int mTileSize = 0;
	glm::vec3 mMapSize = {0, 0, 1};
	std::vector<int> mData;
};
