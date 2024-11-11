#include <cereal/archives/json.hpp>

#include "Descriptions/DescriptionFactory.h"
#include "Util/FileSystem.h"

#include "EntityTemplateRegistry.h"

namespace fs = std::filesystem;

EntityTemplateRegistry::EntityTemplateRegistry(const DescriptionFactory& descriptionFactory)
{
	RegisterEntityTemplates(descriptionFactory);
}

const std::vector<std::shared_ptr<DescriptionBase>>& EntityTemplateRegistry::GetEntityTemplateDescriptions(
	const std::string& templateName)
{
	const auto it = mEntityTemplates.find(templateName);
	return it->second;
}

void EntityTemplateRegistry::RegisterEntityTemplates(const DescriptionFactory& descriptionFactory)
{
	for (const auto& entry : fs::directory_iterator(ENTITIES_FOLDER))
	{
		std::string fullFileName = entry.path().string();
		const auto entityJson = FileSystem::ReadJson(fullFileName);

		std::string templateName = entityJson[TEMPLATE_NAME_STRING];

		std::vector<std::shared_ptr<DescriptionBase>> descriptions;
		for (const auto& [key, value] : entityJson[COMPONENTS_STRING].items())
		{
			descriptions.push_back(descriptionFactory.CreateDescription(key, value.dump()));
		}
		mEntityTemplates.insert({templateName, descriptions});
	}
}
