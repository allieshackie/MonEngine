#pragma once
#include <nlohmann/json.hpp>
#include "Entity/EntityRegistry.h"

struct DescriptionBase
{
public:
	virtual ~DescriptionBase() = default;

	virtual void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) = 0;
	virtual void ParseJSON(const nlohmann::json& jsonFrag) = 0;
};
