#pragma once

struct DescriptionBase;

class EntityTemplateRegistry
{
public:
	EntityTemplateRegistry();
	virtual ~EntityTemplateRegistry() = default;

	std::vector<std::shared_ptr<DescriptionBase>>& GetEntityTemplateDescriptions(const std::string& templateName);

private:
	void _RegisterEntityTemplates();

	const char* mEntitiesFilePath = "../Data/Entities/";
	const char* TEMPLATE_NAME_STRING = "template_name";
	const char* COMPONENTS_STRING = "components";

	std::unordered_map<std::string, std::vector<std::shared_ptr<DescriptionBase>>> mEntityTemplates;
};
