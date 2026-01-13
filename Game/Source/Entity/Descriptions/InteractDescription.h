#pragma once
#include "Entity/Descriptions/DescriptionBase.h"

class InteractDescription : public DescriptionBase
{
public:
	InteractDescription() = default;
	// Copy constructor/assignment operator
	InteractDescription(const InteractDescription& other) = default;
	InteractDescription& operator=(const InteractDescription& other) = default;
	// Move constructor/assignment operator
	InteractDescription(InteractDescription&& other) noexcept = default;
	InteractDescription& operator=(InteractDescription&& rhs) noexcept = default;

	~InteractDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "interact";
};
