#include <filesystem>

#include <nlohmann/json.hpp>
#include "DescriptionFactory.h"

#include "EntityTemplateRegistry.h"

namespace fs = std::filesystem;

EntityTemplateRegistry::EntityTemplateRegistry()
{
	_RegisterEntityTemplates();
}

std::vector<std::shared_ptr<DescriptionBase>>& EntityTemplateRegistry::GetEntityTemplateDescriptions(
	const std::string& templateName)
{
	const auto it = mEntityTemplates.find(templateName);
	return it->second;
}

void EntityTemplateRegistry::_RegisterEntityTemplates()
{
	const auto descriptionFactory = std::make_unique<DescriptionFactory>();
	std::vector<char*> fileNames;
	for (const auto& entry : fs::directory_iterator(mEntitiesFilePath))
	{
		fileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}

	for (const auto fileName : fileNames)
	{
		// parse and serialize JSON
		std::string fullFileName = mEntitiesFilePath;
		fullFileName += fileName;

		std::ifstream ifs(fullFileName.c_str());

		auto json = nlohmann::json::parse(ifs, nullptr, false, true);

		std::string templateName = json[TEMPLATE_NAME_STRING];
		std::vector<std::shared_ptr<DescriptionBase>> descriptions;
		for (auto& [key, value] : json[COMPONENTS_STRING].items())
		{
			descriptions.push_back(descriptionFactory->CreateDescription(key, value));
		}

		mEntityTemplates.insert({templateName, descriptions});

		ifs.close();
	}
}
