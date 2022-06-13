#include "DescriptionRegistry.h"
#include "MapDescription.h"
#include "TileSetDescription.h"

#include "Map.h"

Map::Map()
{
	LoadTiles();
}

void Map::LoadTiles()
{
	auto tileDescription = DescriptionRegistry::getDescription<TileSetDescription>(TileSetDescription::JsonName);
	auto mapDescription = DescriptionRegistry::getDescription<MapDescription>(MapDescription::JsonName);

	const int TILE_WIDTH = tileDescription->getTileWidth();
	const int TILE_HEIGHT = tileDescription->getTileHeight();

	const int MAP_WIDTH = mapDescription->getMapWidth();
	mMapTexture = tileDescription->getTexturePath();
	
	int screenPosX = 0, screenPosY = 0;
	int widthCounter = 1;

	for (const auto& tile : mapDescription->getTiles()) {
		const auto& pos = tileDescription->getTileClipPosition(tile);
		mMapTiles.emplace_back(pos[0], pos[1], TILE_WIDTH, TILE_HEIGHT, screenPosX, screenPosY);

		screenPosX += TILE_WIDTH;
		if (widthCounter == MAP_WIDTH) {
			screenPosX = 0;
			widthCounter = 1;
			screenPosY += TILE_HEIGHT;
			continue;
		}
		widthCounter++;
	}
}

const std::vector<Tile>& Map::GetMapTiles() const
{
	return mMapTiles;
}

const std::string& Map::GetMapTexture() const
{
	return mMapTexture;
}
