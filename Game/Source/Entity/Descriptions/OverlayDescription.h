#pragma once
#include "Entity/Descriptions/DescriptionBase.h"

class OverlayDescription : public DescriptionBase
{
public:
	OverlayDescription() = default;
	// Copy constructor/assignment operator
	OverlayDescription(const OverlayDescription& other) = default;
	OverlayDescription& operator=(const OverlayDescription& other) = default;
	// Move constructor/assignment operator
	OverlayDescription(OverlayDescription&& other) noexcept = default;
	OverlayDescription& operator=(OverlayDescription&& rhs) noexcept = default;

	~OverlayDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "overlay";
};
