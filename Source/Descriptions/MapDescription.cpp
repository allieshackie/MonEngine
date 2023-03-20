#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Components/MapComponent.h"

#include "MapDescription.h"

void MapDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	const glm::vec3 mapSize = {mColumns * mTileSize, mRows * mTileSize, 1};

	entityRegistry.AddComponent<MapComponent>(entity, mRows, mColumns, mTileSize, mapSize, mData);
}

void MapDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(ROWS_STRING));
	mRows = json[ROWS_STRING];

	assert(json.contains(COLUMNS_STRING));
	mColumns = json[COLUMNS_STRING];

	assert(json.contains(TILE_SIZE_STRING));
	mTileSize = json[TILE_SIZE_STRING];

	assert(json.contains(DATA_STRING));
	const auto data = json[DATA_STRING];
	for (auto& dataEntry : data)
	{
		mData.push_back(dataEntry);
	}
}
