#pragma once
#include "MapDescription.h"
#include "Tile.h"


class Map {
public:
	Map(glm::vec2 position, bool isInteractable = false);
	~Map() = default;

	void LoadMap(const char* fileName);

	glm::vec2 GetMapPosition() const;
	int GetMapWidth() const;
	int GetMapHeight() const;

	int GetMapRows() const;
	int GetMapColumns() const;

	const MapDescription& GetMapDescription() const;
	const TileSetDescription& GetTileSetDescription() const;

	void LoadTiles();

private:
	std::vector<Tile> mMapTiles;

	glm::vec2 mMapPosition;
	int mMapWidth = 0;
	int mMapHeight = 0;
	bool mIsInteractable = false;

	MapDescription* mMapDescription = nullptr;
};