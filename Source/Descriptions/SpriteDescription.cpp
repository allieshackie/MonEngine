#include "SpriteComponent.h"

#include "SpriteDescription.h"

void SpriteDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	// TexClip + TexScale ?
	entityRegistry.AddComponent<SpriteComponent>(entity, mResourcePath);
}

void SpriteDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(RESOURCE_PATH_STRING));
	mResourcePath = json[RESOURCE_PATH_STRING];

	if (json.contains(ROWS_STRING))
	{
		mRows = json[ROWS_STRING];
	}

	if (json.contains(COLUMNS_STRING))
	{
		mColumns = json[COLUMNS_STRING];
	}
}
