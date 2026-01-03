#include "Entity/Components/InteractiveComponent.h"

#include "InteractiveDescription.h"

void InteractiveDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	entity->AddComponentWithArgs<InteractiveComponent>();
}
