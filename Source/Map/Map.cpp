#include "DescriptionRegistry.h"
#include "MapDescription.h"
#include "TileSetDescription.h"

#include "Map.h"

#include "Core/ResourceManager.h"

Map::Map()
{
	LoadTiles();
}

void Map::LoadMap(const char* fileName)
{
	mMapDescription = new MapDescription(fileName);
}

void Map::LoadTiles()
{
	const int MAP_WIDTH = mMapDescription->GetMapWidth();
	const int MAP_HEIGHT = mMapDescription->GetMapHeight();

	const auto rawTiles = mMapDescription->GetRawTiles();
	const auto texture = mMapDescription->GetTilesetTexture();
	std::vector<int> splitTiles;
	const std::string del = " ";
	int start = 0;
	int end = rawTiles.find(del);
	while (end != -1) {
		splitTiles.push_back(std::stoi(rawTiles.substr(start, end - start)));
		start = end + del.size();
		end = rawTiles.find(del, start);
	}
	
	int screenPosX = 0, screenPosY = 0;
	int widthCounter = 1;

	for (const auto& tile : splitTiles) {
		// TODO: Get texture clip and size 
		ResourceManager::CreateTile(texture, {screenPosX, screenPosY}, {10,10});

		screenPosX += 10;
		if (widthCounter == MAP_WIDTH) {
			screenPosX = 0;
			widthCounter = 1;
			screenPosY += 10;
			continue;
		}
		widthCounter++;
	}
	 
}
