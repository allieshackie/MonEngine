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

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;
	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "player";
};
