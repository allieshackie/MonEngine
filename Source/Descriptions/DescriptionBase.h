#pragma once
#include <nlohmann/json.hpp>

class EntityContext;

struct ComponentDescriptionBase
{
public:
	virtual ~ComponentDescriptionBase() {}

	virtual void applyToEntity(EntityContext& entity) = 0;
	virtual void parseJSON(const nlohmann::json& jsonFrag) = 0;
};

struct DescriptionBase {
public:
	virtual ~DescriptionBase() {}

	virtual void applyToEntity(EntityContext& entity) = 0;

private:
	virtual void parseJSON() = 0;
}; 