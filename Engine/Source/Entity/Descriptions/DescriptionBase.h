#pragma once
#include <nlohmann/json.hpp>
#include "Entity/EntityRegistry.h"

class DescriptionBase
{
public:
	DescriptionBase() = default;
	// Copy constructor/assignment operator
	DescriptionBase(const DescriptionBase& other) = default;
	DescriptionBase& operator=(const DescriptionBase& other) = default;
	// Move constructor/assignment operator
	DescriptionBase(DescriptionBase&& other) noexcept = default;
	DescriptionBase& operator=(DescriptionBase&& rhs) noexcept = default;

	virtual ~DescriptionBase() = default;

	virtual void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) = 0;
	virtual void ParseJSON(const nlohmann::json& jsonFrag) = 0;
};
