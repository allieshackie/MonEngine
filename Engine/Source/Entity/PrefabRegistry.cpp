#include <cereal/archives/json.hpp>
#include "Util/FileSystem.h"

#include "PrefabRegistry.h"

namespace fs = std::filesystem;

void PrefabRegistry::RegisterPrefabs()
{
	for (const auto& entry : fs::directory_iterator(PREFABS_FOLDER))
	{
		std::string fullFileName = entry.path().string();
		const auto entityJson = FileSystem::ReadJson(fullFileName);

		std::string templateName = entityJson[PREFAB_NAME_STRING];

		std::vector<SerializedComponent> components;
		for (const auto& [key, value] : entityJson[COMPONENTS_STRING].items())
		{
			components.push_back({key, mComponentLoaders[key], value});
		}
		mPrefabs.insert({templateName, components });
	}
}
