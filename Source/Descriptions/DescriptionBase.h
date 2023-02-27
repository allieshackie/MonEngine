#pragma once
#include <nlohmann/json.hpp>
#include "EntityRegistry.h"

struct DescriptionBase
{
public:
	virtual ~DescriptionBase() = default;

	virtual void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) = 0;
	virtual void ParseJSON(const nlohmann::json& jsonFrag) = 0;
};
