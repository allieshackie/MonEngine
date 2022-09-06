#pragma once
#include <nlohmann/json.hpp>

class EntityContext;

struct ComponentDescriptionBase
{
public:
	virtual ~ComponentDescriptionBase() = default;

	virtual void ApplyToEntity(EntityContext& entity) = 0;
	virtual void ParseJSON(const nlohmann::json& jsonFrag) = 0;
};

struct DescriptionBase {
public:
	virtual ~DescriptionBase() = default;

	virtual void ApplyToEntity(EntityContext& entity) = 0;

private:
	virtual void ParseJSON(const char* fileName) = 0;
}; 