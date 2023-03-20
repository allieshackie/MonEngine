#pragma once

#include "DescriptionBase.h"

struct InteractiveDescription : DescriptionBase
{
public:
	InteractiveDescription() = default;
	~InteractiveDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;
	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "interactive";
};
