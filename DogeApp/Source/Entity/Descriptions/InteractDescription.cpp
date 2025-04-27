#include "Entity/Components/InteractComponent.h"

#include "InteractDescription.h"

void InteractDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	InteractComponent interact;
	entity->AddComponent<InteractComponent>(interact);
}
