#pragma once

#include "Entity/Descriptions/DescriptionBase.h"

class BehaviorDescription : public DescriptionBase
{
public:
	BehaviorDescription() = default;
	// Copy constructor/assignment operator
	BehaviorDescription(const BehaviorDescription& other) = default;
	BehaviorDescription& operator=(const BehaviorDescription& other) = default;
	// Move constructor/assignment operator
	BehaviorDescription(BehaviorDescription&& other) noexcept = default;
	BehaviorDescription& operator=(BehaviorDescription&& rhs) noexcept = default;

	~BehaviorDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "behavior";
};
