#pragma once

#include "DescriptionBase.h"
#include "TileSetDescription.h"

#include <glm/glm.hpp>

struct MapDescription : DescriptionBase {
public: 
	MapDescription(const char* fileName);
	~MapDescription() override = default;

	void Load(const char* fileName);

	int GetMapWidth() const;
	int GetMapHeight() const;

	const std::string& GetRawTiles();

	const std::string& GetTilesetTexture();

	virtual void applyToEntity(EntityContext& entity) override {}

private:
	virtual void parseJSON(const char* fileName) override;

public:
	static constexpr char JsonName[] = "mondev:map";
private:
	static constexpr char JSON_PATH[] = "../Data/Maps/";
	static constexpr char MAP_STRING[] = "map";
	static constexpr char TILESET_STRING[] = "tileset";
	static constexpr char FILE_STRING[] = "file";
	static constexpr char SIZE_STRING[] = "size";

	int mWidth = 0;
	int mHeight = 0;
	std::string mMapTextFile;
	std::string mTiles;

	TileSetDescription* mTilesetDescription = nullptr;
};