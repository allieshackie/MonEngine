#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

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

	glm::vec4 GetClipForTile(int index) const;

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
	static constexpr char TEXTURE_SIZE_STRING[] = "texture_size";

	std::string mTexturePath;
	int mTilesetRows = 0;
	int mTilesetColumns = 0;

	glm::vec2 mTextureSize = {1.0f, 1.0f};
};