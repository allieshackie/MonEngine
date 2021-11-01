#pragma once

#include <iostream>
#include <vector>

#include "DescriptionBase.h"

struct MapDescription : DescriptionBase {
public: 
	MapDescription();
	~MapDescription() {}

	const std::vector<std::string>& getTiles();
	int getMapWidth();

	virtual void applyToEntity(EntityContext& entity) override {}

private:
	virtual void parseJSON() override;

public:
	static constexpr char JsonName[] = "mondev:map";
private:
	static constexpr char JSON_PATH[] = "C:/dev/MonDev/Data/Maps/default_map.json";
	static constexpr char MAP_STRING[] = "map";
	static constexpr char WIDTH_STRING[] = "width";
	static constexpr char HEIGHT_STRING[] = "height";
	static constexpr char TILES_STRING[] = "tiles";

	int mWidth = 0;
	int mHeight = 0;
	std::vector<std::string> mTiles;
};