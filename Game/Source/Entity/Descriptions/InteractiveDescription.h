#pragma once

#include "Entity/Descriptions/DescriptionBase.h"

class InteractiveDescription : public DescriptionBase
{
public:
	InteractiveDescription() = default;
	// Copy constructor/assignment operator
	InteractiveDescription(const InteractiveDescription& other) = default;
	InteractiveDescription& operator=(const InteractiveDescription& other) = default;
	// Move constructor/assignment operator
	InteractiveDescription(InteractiveDescription&& other) noexcept = default;
	InteractiveDescription& operator=(InteractiveDescription&& rhs) noexcept = default;

	~InteractiveDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "interactive";
};
