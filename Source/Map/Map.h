#pragma once
#include "MapDescription.h"
#include "Tile.h"


class Map {
public:
	Map();
	~Map() = default;

	void LoadMap(const char* fileName);

	void LoadTiles();

private:
	std::vector<Tile> mMapTiles;
	int mMapTextureId = 0;

	MapDescription* mMapDescription = nullptr;
};