#include "Components/PlayerComponent.h"

#include "PlayerDescription.h"

void PlayerDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<PlayerComponent>(entity);
}

void PlayerDescription::ParseJSON(const nlohmann::json& json)
{
}
