#pragma once

#include "DescriptionBase.h"

class SpriteDescription : public DescriptionBase
{
public:
	SpriteDescription() = default;
	// Copy constructor/assignment operator
	SpriteDescription(const SpriteDescription& other) = default;
	SpriteDescription& operator=(const SpriteDescription& other) = default;
	// Move constructor/assignment operator
	SpriteDescription(SpriteDescription&& other) noexcept = default;
	SpriteDescription& operator=(SpriteDescription&& rhs) noexcept = default;

	~SpriteDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override = 0;

	static constexpr char JsonName[] = "sprite";
};
