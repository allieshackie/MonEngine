#pragma once
#include <SDL.h>
#include "Tile.h"
#include "Texture.h"

class DescriptionRegistry;

class Map {
public:
	Map(DescriptionRegistry& registry, SDL_Renderer& renderer);
	~Map();

	void LoadTiles();
	void Render();
	void AnimateTiles();

private:
	DescriptionRegistry& mDescriptionRegistry;
	SDL_Renderer& mRenderer;
	Texture* mMapTexture;
	std::vector<Tile> mMapTiles;
	bool mAnimateUp = true;
	float mAnimateTimer = 0.0f;
};