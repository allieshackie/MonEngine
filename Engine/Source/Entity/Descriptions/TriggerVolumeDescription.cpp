#include "Entity/Components/TriggerVolumeComponent.h"

#include "TriggerVolumeDescription.h"

void TriggerVolumeDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	entity->AddComponentWithArgs<TriggerVolumeComponent>();
}
