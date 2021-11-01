#pragma once

class EntityContext;

struct DescriptionBase {
public:
	virtual ~DescriptionBase() {}

	virtual void applyToEntity(EntityContext& entity) = 0;

private:
	virtual void parseJSON() = 0;
}; 