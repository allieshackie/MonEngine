#pragma once
#include <vector>
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

private:
	DescriptionRegistry& mDescriptionRegistry;
	SDL_Renderer& mRenderer;
	Texture* mMapTexture;
	std::vector<Tile> mMapTiles;
};