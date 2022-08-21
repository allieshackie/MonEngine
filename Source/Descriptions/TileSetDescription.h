#pragma once

#include "DescriptionBase.h"

/*
	JSON Parser can't handle comments, putting relevant information here:
	
	ground_tileset.json
	320px x 240px
	20 x 16 division
	tiles: 16 x 15px
*/

struct TileSetDescription : DescriptionBase {
public:
	TileSetDescription(const char* fileName);

	void Load(const char* fileName);

	virtual void applyToEntity(EntityContext& entity) override {}

	const std::string& getTexturePath() const;
	int getTileWidth() const;
	int getTileHeight() const;

private:
	virtual void parseJSON(const char* fileName) override;

public:
	static constexpr char JsonName[] = "mondev:tileset";
private:
	static constexpr char JSON_PATH[] = "../Data/Tileset/";
	static constexpr char TILESET_STRING[] = "tileset";
	static constexpr char ID_STRING[] = "id";
	static constexpr char TEXTURE_STRING[] = "texture_path";
	static constexpr char SIZE_STRING[] = "size";

	std::string mTexturePath;
	int mTileWidth = 0;
	int mTileHeight = 0;
};