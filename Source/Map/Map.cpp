#include "DescriptionRegistry.h"
#include "MapDescription.h"
#include "Texture.h"
#include "TileSetDescription.h"
#include "Tile.h"

#include "Map.h"

const float ANIMATE_TIMER = 0.5f;

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

	//mMapTexture = new Texture(mRenderer, tileDescription->getTexturePath());

	const int TILE_WIDTH = tileDescription->getTileWidth();
	const int TILE_HEIGHT = tileDescription->getTileHeight();

	const int MAP_WIDTH = mapDescription->getMapWidth();
	
	int screenPosX = 0, screenPosY = 0;
	int widthCounter = 1;

	for (const auto& tile : mapDescription->getTiles()) {
		const auto& pos = tileDescription->getTileClipPosition(tile);
		mMapTiles.push_back({ pos[0], pos[1], TILE_WIDTH, TILE_HEIGHT, screenPosX, screenPosY });

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

void Map::AnimateTiles() {
	mAnimateTimer -= 0.001f;
	if (mAnimateTimer <= 0.0f) {
		mAnimateTimer = ANIMATE_TIMER;
		for (auto& tile : mMapTiles) {
			if (mAnimateUp) {
				tile.updateScreenPosX(20);
			}
			else {
				tile.updateScreenPosX(-20);
			}
		}

		mAnimateUp = !mAnimateUp;
	}
}

void Map::Render()
{
	for (auto& tile : mMapTiles) {
		//mMapTexture->Render(tile.getScreenPosX(), tile.getScreenPosY(), &tile.getBox());
	}
}
