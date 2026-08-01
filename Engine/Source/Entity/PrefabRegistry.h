#pragma once
#include "Util/FileSystem.h" // include for inherited classes

class Entity;

using ComponentLoader = std::function<void(Entity*, const nlohmann::json&)>;
using ComponentSaver = std::function<bool(Entity*, nlohmann::json&)>;

struct ComponentSaveLoad
{
	ComponentLoader loader;
	ComponentSaver saver;
};

struct SerializedComponent
{
	std::string key;
	ComponentSaveLoad saveLoader;
	nlohmann::json json;
};

class PrefabRegistry
{
public:
	template<typename T>
	void RegisterComponent(const char* componentTag)
	{
		mComponentLoaders[componentTag].loader =
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

		mComponentLoaders[componentTag].saver =
			[](Entity* entity, nlohmann::json& out)
			{
				if (!entity->HasComponent<T>())
				{
					return false;
				}

				auto& component = entity->GetComponent<T>();

				std::stringstream stream;
				{
					cereal::JSONOutputArchive archive(stream);
					component.serialize(archive);
				}
				if (stream.str().empty())
				{
					return false;
				}
				out = nlohmann::json::parse(stream.str());
				return true;
			};
	}

	void RegisterPrefabs();

	std::vector<SerializedComponent>& GetPrefabComponents(const std::string& prefabName) { return mPrefabs[prefabName]; }
	const ComponentSaveLoad& GetComponentLoader(const std::string& componentTag) { return mComponentLoaders.find(componentTag)->second; }
	const std::unordered_map<std::string, ComponentSaveLoad>& GetAllComponentLoaders() { return mComponentLoaders; }

private:
	const char* PREFAB_NAME_STRING = "prefab_name";
	const char* COMPONENTS_STRING = "components";

	std::unordered_map<std::string, ComponentSaveLoad> mComponentLoaders;
	std::unordered_map<std::string, std::vector<SerializedComponent>> mPrefabs;
};