#pragma once

class DescriptionBase;
class DescriptionFactory;

class EntityTemplateRegistry
{
public:
	EntityTemplateRegistry(const DescriptionFactory& descriptionFactory);

	void RegisterEntityTemplates(const DescriptionFactory& descriptionFactory);
	const std::vector<std::shared_ptr<DescriptionBase>>& GetEntityTemplateDescriptions(const std::string& templateName);

private:
	const char* TEMPLATE_NAME_STRING = "template_name";
	const char* COMPONENTS_STRING = "components";

	std::unordered_map<std::string, std::vector<std::shared_ptr<DescriptionBase>>> mEntityTemplates;
};
