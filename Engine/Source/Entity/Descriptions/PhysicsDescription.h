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

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "rigidbody";
};
