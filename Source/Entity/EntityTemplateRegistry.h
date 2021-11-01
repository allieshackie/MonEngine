#pragma once

class EntityTemplateRegistry {
public:
	EntityTemplateRegistry() = default;
	EntityTemplateRegistry(const EntityTemplateRegistry& other) = delete;
	EntityTemplateRegistry(EntityTemplateRegistry&& other) = default;
	virtual ~EntityTemplateRegistry() = default;
};