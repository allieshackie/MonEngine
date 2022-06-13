#pragma once

class EntityTemplateRegistry {
public:
	EntityTemplateRegistry() = default;
	virtual ~EntityTemplateRegistry() = default;

private:
	void _registerEntityTemplates();

	const char* entitiesFilePath = "";
};