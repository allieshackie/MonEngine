#include <cereal/archives/json.hpp>

#include "Descriptions/DescriptionFactory.h"
#include "Util/FileSystem.h"

#include "PrefabRegistry.h"

namespace fs = std::filesystem;

PrefabRegistry::PrefabRegistry(const DescriptionFactory& descriptionFactory)
{
	RegisterPrefabs(descriptionFactory);
}

const std::vector<std::shared_ptr<DescriptionBase>>& PrefabRegistry::GetPrefabsDescriptions(
	const std::string& templateName)
{
	const auto it = mPrefabs.find(templateName);
	return it->second;
}

void PrefabRegistry::RegisterPrefabs(const DescriptionFactory& descriptionFactory)
{
	for (const auto& entry : fs::directory_iterator(PREFABS_FOLDER))
	{
		std::string fullFileName = entry.path().string();
		const auto entityJson = FileSystem::ReadJson(fullFileName);

		std::string templateName = entityJson[PREFAB_NAME_STRING];

		std::vector<std::shared_ptr<DescriptionBase>> descriptions;
		for (const auto& [key, value] : entityJson[COMPONENTS_STRING].items())
		{
			descriptions.push_back(descriptionFactory.CreateDescription(key, value.dump()));
		}
		mPrefabs.insert({templateName, descriptions});
	}
}
