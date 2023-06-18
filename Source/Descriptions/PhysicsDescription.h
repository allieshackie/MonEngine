#pragma once

#include "DescriptionBase.h"

struct PhysicsDescription : DescriptionBase
{
public:
	PhysicsDescription() = default;
	~PhysicsDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;

	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "rigidbody";

private:
	static constexpr char MASS_STRING[] = "mass";
	static constexpr char MAX_SPEED_STRING[] = "max_speed";
	static constexpr char ACCELERATION_RATE_STRING[] = "acceleration_rate";

	float mMass = 0.0f;
	float mMaxSpeed = 0.0f;
	float mAccelerationRate = 0.0f;
};
