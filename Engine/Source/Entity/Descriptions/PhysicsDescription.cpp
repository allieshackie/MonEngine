#include <nlohmann/json.hpp>
#include "Entity/EntityRegistry.h"
#include "Entity/Components/PhysicsComponent.h"

#include "PhysicsDescription.h"

void PhysicsDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<PhysicsComponent>(entity, mMass, mMaxSpeed, mAccelerationRate, mFriction, mIsDynamic);
}

void PhysicsDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(MASS_STRING));
	mMass = json[MASS_STRING];

	assert(json.contains(MAX_SPEED_STRING));
	mMaxSpeed = json[MAX_SPEED_STRING];

	assert(json.contains(ACCELERATION_RATE_STRING));
	mAccelerationRate = json[ACCELERATION_RATE_STRING];

	if (json.contains(IS_DYNAMIC_STRING))
	{
		mIsDynamic = json[IS_DYNAMIC_STRING];
	}

	if (json.contains(FRICTION_STRING))
	{
		mFriction = json[FRICTION_STRING];
	}
}
