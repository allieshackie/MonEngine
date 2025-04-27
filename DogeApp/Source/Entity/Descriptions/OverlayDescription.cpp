#include "Entity/Components/OverlayComponent.h"

#include "OverlayDescription.h"

void OverlayDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	OverlayComponent overlay;
	entity->AddComponent<OverlayComponent>(overlay);
}
