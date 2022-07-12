#pragma once
#include "Tile.h"

class Map {
public:
	Map();
	~Map() = default;

	void LoadTiles();

	const std::vector<Tile>& GetMapTiles() const;
	const std::string& GetMapTexture() const;

private:
	std::vector<Tile> mMapTiles;
	int mMapTextureId;
};