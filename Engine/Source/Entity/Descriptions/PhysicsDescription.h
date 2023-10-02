#pragma once

#include "Entity/Descriptions/DescriptionBase.h"

class PhysicsDescription : public DescriptionBase
{
public:
	PhysicsDescription() = default;
	// Copy constructor/assignment operator
	PhysicsDescription(const PhysicsDescription& other) = default;
	PhysicsDescription& operator=(const PhysicsDescription& other) = default;
	// Move constructor/assignment operator
	PhysicsDescription(PhysicsDescription&& other) noexcept = default;
	PhysicsDescription& operator=(PhysicsDescription&& rhs) noexcept = default;

	~PhysicsDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;
	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "rigidbody";

private:
	static constexpr char MASS_STRING[] = "mass";
	static constexpr char MAX_SPEED_STRING[] = "max_speed";
	static constexpr char ACCELERATION_RATE_STRING[] = "acceleration_rate";
	static constexpr char APPLY_GRAVITY_STRING[] = "apply_gravity";

	float mMass = 0.0f;
	float mMaxSpeed = 0.0f;
	float mAccelerationRate = 0.0f;
	bool mApplyGravity = true;
};
