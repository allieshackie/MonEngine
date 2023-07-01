#include <nlohmann/json.hpp>
#include "Entity/EntityRegistry.h"
#include "Entity/Components/InteractiveComponent.h"

#include "InteractiveDescription.h"

void InteractiveDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<InteractiveComponent>(entity);
}

void InteractiveDescription::ParseJSON(const nlohmann::json& json)
{
}
