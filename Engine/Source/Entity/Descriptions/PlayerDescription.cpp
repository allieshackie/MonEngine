#include "Entity/Components/PlayerComponent.h"

#include "PlayerDescription.h"

void PlayerDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	PlayerComponent player;
	entity->AddComponent<PlayerComponent>(player);
}
