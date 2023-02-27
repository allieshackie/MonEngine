#include "MapComponent.h"

#include "MapDescription.h"

void MapDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<MapComponent>(entity, mRows, mColumns, mTileSize);
}

void MapDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(ROWS_STRING));
	mRows = json[ROWS_STRING];

	assert(json.contains(COLUMNS_STRING));
	mColumns = json[COLUMNS_STRING];

	assert(json.contains(TILE_SIZE_STRING));
	mTileSize = json[TILE_SIZE_STRING];
}
