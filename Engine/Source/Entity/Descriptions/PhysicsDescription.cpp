#include <nlohmann/json.hpp>
#include "Entity/EntityRegistry.h"
#include "Entity/Components/PhysicsComponent.h"

#include "PhysicsDescription.h"

void PhysicsDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<PhysicsComponent>(entity, mMass, mFriction);
}

void PhysicsDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(MASS_STRING));
	mMass = json[MASS_STRING];

	if (json.contains(FRICTION_STRING))
	{
		mFriction = json[FRICTION_STRING];
	}
}
