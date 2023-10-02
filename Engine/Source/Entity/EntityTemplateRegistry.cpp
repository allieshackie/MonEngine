#include <filesystem>
#include <nlohmann/json.hpp>

#include "Core/FileSystem.h"
#include "Descriptions/DescriptionFactory.h"

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
	std::vector<char*> fileNames;
	for (const auto& entry : fs::directory_iterator(ENTITIES_FOLDER))
	{
		fileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}

	for (const auto fileName : fileNames)
	{
		// parse and serialize JSON
		std::string fullFileName = ENTITIES_FOLDER;
		fullFileName += fileName;

		auto json = FileSystem::ReadJson(fullFileName);

		std::string templateName = json[TEMPLATE_NAME_STRING];
		std::vector<std::shared_ptr<DescriptionBase>> descriptions;
		for (auto& [key, value] : json[COMPONENTS_STRING].items())
		{
			descriptions.push_back(descriptionFactory.CreateDescription(key, value));
		}

		mEntityTemplates.insert({templateName, descriptions});
	}
}
