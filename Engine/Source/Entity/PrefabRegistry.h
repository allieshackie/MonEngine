#pragma once
#include "Util/FileSystem.h" // include for inherited classes

class Entity;

using ComponentLoader = std::function<void(Entity*, const nlohmann::json&)>;
struct SerializedComponent
{
	std::string key;
	ComponentLoader loader;
	nlohmann::json json;
};

class PrefabRegistry
{
public:
	template<typename T>
	void RegisterComponent(const char* componentTag)
	{
		mComponentLoaders[componentTag] =
			[](Entity* entity, const nlohmann::json& json)
			{
				auto& component = entity->AddComponent<T>();

				try
				{
					std::istringstream stream(json.dump());

					cereal::JSONInputArchive archive(stream);

					component.serialize(archive);
				}
				catch (const cereal::RapidJSONException& e)
				{
					std::cout << e.what() << std::endl;
				}
			};
	}

	void RegisterPrefabs();

	std::vector<SerializedComponent>& GetPrefabComponents(const std::string& prefabName) { return mPrefabs[prefabName]; }
	const ComponentLoader& GetComponentLoader(const std::string& componentTag) { return mComponentLoaders.find(componentTag)->second; }

private:
	const char* PREFAB_NAME_STRING = "prefab_name";
	const char* COMPONENTS_STRING = "components";

	std::unordered_map<std::string, ComponentLoader> mComponentLoaders;
	std::unordered_map<std::string, std::vector<SerializedComponent>> mPrefabs;
};