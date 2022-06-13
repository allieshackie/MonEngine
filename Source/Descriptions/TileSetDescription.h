#pragma once

#include "DescriptionBase.h"

/*
	JSON Parser can't handle comments, putting relevant information here:
	
	ground_tileset.json
	320px x 240px
	20 x 16 division
	tiles: 16 x 15px
*/

struct TileSet {
	std::string mId;
	int mWidth = 0;
	int mHeight = 0;
};

struct TileData {
	std::string mId;
	int mClipPosX = 0;
	int mClipPosY = 0;
};

struct TileSetDescription : DescriptionBase {
public:
	TileSetDescription();

	virtual void applyToEntity(EntityContext& entity) override {}

	const std::string& getTexturePath() const;
	int getTileWidth() const;
	int getTileHeight() const;
	std::vector<int> getTileClipPosition(const std::string& tileId);

private:
	virtual void parseJSON() override;

public:
	static constexpr char JsonName[] = "mondev:tileset";
private:
	static constexpr char JSON_PATH[] = "C:/dev/MonDev/Data/Tileset/color_tileset.json";
	static constexpr char TILESET_STRING[] = "tileset";
	static constexpr char ID_STRING[] = "id";
	static constexpr char TEXTURE_STRING[] = "texture_path";
	static constexpr char WIDTH_STRING[] = "width";
	static constexpr char HEIGHT_STRING[] = "height";

	static constexpr char TILE_DATA_STRING[] = "tile_data";

	static constexpr char TILES_STRING[] = "tiles";

	TileSet mTileSet;
	std::string mTexturePath;
	std::vector<TileData> mTilesData;
	int mTileWidth = 0;
	int mTileHeight = 0;
};