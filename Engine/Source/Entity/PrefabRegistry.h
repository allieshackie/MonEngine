#pragma once

class DescriptionBase;
class DescriptionFactory;

class PrefabRegistry
{
public:
	PrefabRegistry(const DescriptionFactory& descriptionFactory);

	void RegisterPrefabs(const DescriptionFactory& descriptionFactory);
	const std::vector<std::shared_ptr<DescriptionBase>>& GetPrefabsDescriptions(const std::string& templateName);

private:
	const char* PREFAB_NAME_STRING = "prefab_name";
	const char* COMPONENTS_STRING = "components";

	std::unordered_map<std::string, std::vector<std::shared_ptr<DescriptionBase>>> mPrefabs;
};
