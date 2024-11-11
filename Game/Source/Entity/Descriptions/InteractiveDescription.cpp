#include "Entity/Components/InteractiveComponent.h"

#include "InteractiveDescription.h"

void InteractiveDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	InteractiveComponent interactive;
	entityRegistry.AddComponent<InteractiveComponent>(entity, interactive);
}
