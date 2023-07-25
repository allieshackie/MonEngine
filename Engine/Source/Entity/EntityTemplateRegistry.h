#pragma once

struct DescriptionBase;
class DescriptionFactory;

class EntityTemplateRegistry
{
public:
	EntityTemplateRegistry(DescriptionFactory& descriptionFactory, const std::string& path);
	virtual ~EntityTemplateRegistry() = default;

	void RegisterEntityTemplates(const std::string& path);
	std::vector<std::shared_ptr<DescriptionBase>>& GetEntityTemplateDescriptions(const std::string& templateName);

private:
	const char* TEMPLATE_NAME_STRING = "template_name";
	const char* COMPONENTS_STRING = "components";

	std::unordered_map<std::string, std::vector<std::shared_ptr<DescriptionBase>>> mEntityTemplates;

	DescriptionFactory& mDescriptionFactory;
};
