#include "Entity/Components/PlayerComponent.h"

#include "PlayerDescription.h"

void PlayerDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	PlayerComponent player;
	entityRegistry.AddComponent<PlayerComponent>(entity, player);
}
