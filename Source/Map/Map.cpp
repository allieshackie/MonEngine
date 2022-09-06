#include "DescriptionRegistry.h"
#include "MapDescription.h"
#include "TileSetDescription.h"

#include "Map.h"

#include "Core/ResourceManager.h"

void Map::LoadMap(const char* fileName)
{
	mMapDescription = new MapDescription(fileName);
	LoadTiles();
}

void Map::LoadTiles()
{
	const int MAP_WIDTH = mMapDescription->GetMapWidth();
	const int MAP_HEIGHT = mMapDescription->GetMapHeight();

	const auto tiles = mMapDescription->GetTiles();
	const auto texture = mMapDescription->GetTilesetTexture();
	
	int screenPosX = 20, screenPosY = 100;
	int widthCounter = 1;

	// TODO: Using hardcoded size for tiles, try data driving or making dynamic
	for (const auto& tile : tiles) {
		const auto clip = mMapDescription->GetClipForTile(tile);
		ResourceManager::CreateTile(texture, {screenPosX, screenPosY}, {100,30}, glm::vec2(clip.x, clip.y), glm::vec2(clip.z, clip.w));

		screenPosX += 50;
		if (widthCounter == MAP_WIDTH) {
			screenPosX = 20;
			widthCounter = 1;
			screenPosY += 50;
			continue;
		}
		widthCounter++;
	}
	 
}
