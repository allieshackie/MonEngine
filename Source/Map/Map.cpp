#include "Core/ResourceManager.h"
#include "MapDescription.h"

#include "Map.h"

Map::Map(glm::vec2 position, bool isInteractable) : mMapPosition(position), mIsInteractable(isInteractable)
{
}

void Map::LoadMap(const char* fileName)
{
	mMapDescription = new MapDescription(fileName);
	LoadTiles();
}

glm::vec2 Map::GetMapPosition() const
{
	return mMapPosition;
}

int Map::GetMapWidth() const
{
	return mMapWidth;
}

int Map::GetMapHeight() const
{
	return mMapHeight;
}

int Map::GetMapRows() const
{
	return mMapDescription->GetMapRows();
}

int Map::GetMapColumns() const
{
	return mMapDescription->GetMapColumns();
}

const MapDescription& Map::GetMapDescription() const
{
	return *mMapDescription;
}

const TileSetDescription& Map::GetTileSetDescription() const
{
	return mMapDescription->GetTileSetDescription();
}

void Map::LoadTiles()
{
	const int ROWS = mMapDescription->GetMapRows();
	const int COLUMNS = mMapDescription->GetMapColumns();
	const int TILE_SIZE = mMapDescription->GetTileSize();

	mMapWidth = COLUMNS * TILE_SIZE;
	mMapHeight = ROWS * TILE_SIZE;

	const auto tiles = mMapDescription->GetTiles();
	const auto textureName = GetTileSetDescription().getTexturePath();

	for (int i = 0; i < tiles.size(); i++)
	{
		const auto tile = tiles[i];
		const float posX = i % COLUMNS;
		const float currentRow = ceil(i / COLUMNS);
		const auto clip = GetTileSetDescription().GetClipForTile(tile);
		// NOTE: Tile size seems to actually be half of the tile size
		// So we need to step by tilesize * 2 to accurately align
		ResourceManager::GetInstance()->CreateTile(textureName, { mMapPosition.x + (posX * (TILE_SIZE * 2)), mMapPosition.y + (currentRow * (TILE_SIZE * 2)) }, { TILE_SIZE,TILE_SIZE }, glm::vec2(clip.x, clip.y), glm::vec2(clip.z, clip.w), mIsInteractable);
	}
}
