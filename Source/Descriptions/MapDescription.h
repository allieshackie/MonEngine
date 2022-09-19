#pragma once

#include "DescriptionBase.h"
#include "TileSetDescription.h"

#include <glm/glm.hpp>

struct MapDescription : DescriptionBase {
public: 
	MapDescription(const char* fileName);
	~MapDescription() override = default;

	void Load(const char* fileName);

	int GetMapRows() const;
	int GetMapColumns() const;

	int GetMapWidth() const;
	int GetMapHeight() const;

	int GetTileSize() const;

	const std::vector<int>& GetTiles();

	const std::string& GetTilesetTexture();

	// x,y clip    z,w size
	glm::vec4 GetClipForTile(int index) const;

	virtual void ApplyToEntity(EntityContext& entity) override {}

private:
	virtual void ParseJSON(const char* fileName) override;

public:
	static constexpr char JsonName[] = "mondev:map";
private:
	static constexpr char JSON_PATH[] = "../Data/Maps/";
	static constexpr char MAP_STRING[] = "map";
	static constexpr char TILESET_STRING[] = "tileset";
	static constexpr char FILE_STRING[] = "file";
	static constexpr char SIZE_STRING[] = "size";
	static constexpr char TILE_SIZE_STRING[] = "tile_size";

	int mRows = 0;
	int mColumns = 0;

	int mTileSize = 0;

	int mMapWidth = 0;
	int mMapHeight = 0;

	std::string mMapTextFile;
	std::vector<int> mTiles;

	TileSetDescription* mTilesetDescription = nullptr;
};