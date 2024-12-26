#pragma once

#include "Entity/Descriptions/DescriptionBase.h"

class PlayerDescription : public DescriptionBase
{
public:
	PlayerDescription() = default;
	// Copy constructor/assignment operator
	PlayerDescription(const PlayerDescription& other) = default;
	PlayerDescription& operator=(const PlayerDescription& other) = default;
	// Move constructor/assignment operator
	PlayerDescription(PlayerDescription&& other) noexcept = default;
	PlayerDescription& operator=(PlayerDescription&& rhs) noexcept = default;

	~PlayerDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override = 0;

	static constexpr char JsonName[] = "player";
};
