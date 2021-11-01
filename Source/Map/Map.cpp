#include "Map.h"
#include "MapDescription.h"
#include "TileSetDescription.h"
#include "Tile.h"
#include "Texture.h"
#include "DescriptionRegistry.h"

Map::Map(DescriptionRegistry& registry, SDL_Renderer& renderer) : 
	mDescriptionRegistry(registry),
	mRenderer(renderer)
{
	LoadTiles();
}

Map::~Map()
{
}

void Map::LoadTiles()
{
	auto tileDescription = mDescriptionRegistry.getDescription<TileSetDescription>(TileSetDescription::JsonName);
	auto mapDescription = mDescriptionRegistry.getDescription<MapDescription>(MapDescription::JsonName);

	mMapTexture = new Texture(mRenderer, tileDescription->getTexturePath());

	const int TILE_WIDTH = tileDescription->getTileWidth();
	const int TILE_HEIGHT = tileDescription->getTileHeight();

	//The tile offsets
	int x = 0, y = 0;

	const int MAP_WIDTH = mapDescription->getMapWidth();

	for (const auto& tile : mapDescription->getTiles()) {
		mMapTiles.push_back({ x, y, TILE_WIDTH, TILE_HEIGHT });

		//Move to next tile spot
		x += TILE_WIDTH;

		//If we've gone too far
		if (x >= MAP_WIDTH)
		{
			//Move back
			x = 0;

			//Move to the next row
			y += TILE_HEIGHT;
		}
	}
}

void Map::Render()
{
	for (auto& tile : mMapTiles) {
		mMapTexture->Render(tile.getBox().x, tile.getBox().y, &tile.getBox());
	}
}
