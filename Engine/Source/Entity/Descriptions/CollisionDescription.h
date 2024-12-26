#pragma once
#include "Entity/Descriptions/DescriptionBase.h"

class CollisionDescription : public DescriptionBase
{
public:
	CollisionDescription() = default;
	// Copy constructor/assignment operator
	CollisionDescription(const CollisionDescription& other) = default;
	CollisionDescription& operator=(const CollisionDescription& other) = default;
	// Move constructor/assignment operator
	CollisionDescription(CollisionDescription&& other) noexcept = default;
	CollisionDescription& operator=(CollisionDescription&& rhs) noexcept = default;

	~CollisionDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override = 0;

	static constexpr char JsonName[] = "collider";
};
