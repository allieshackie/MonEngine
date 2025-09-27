#include "Entity/Components/InteractiveComponent.h"

#include "InteractiveDescription.h"

void InteractiveDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	InteractiveComponent interactive;
	entity->AddComponent<InteractiveComponent>(interactive);
}
