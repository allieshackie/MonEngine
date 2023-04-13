#pragma once

#include "DescriptionBase.h"

struct PlayerDescription : DescriptionBase
{
public:
	PlayerDescription() = default;
	~PlayerDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;

	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "player";
};
