#pragma once
#include <glm/vec3.hpp>

class Map;

class MapRegistry
{
public:
	MapRegistry() = default;
	~MapRegistry() = default;

	void OpenMap(const std::string& mapPath, glm::vec3 pos, glm::vec3 rotation, float tileSize);
	void OpenMap(const std::string& mapPath);
	void CloseMap(const std::string& mapId);

	const std::vector<std::shared_ptr<Map>>& GetAllMaps();
	const std::shared_ptr<Map>& GetCurrentMap();

private:
	std::vector<std::shared_ptr<Map>> mMaps;

	std::shared_ptr<Map> mEmptyMap = nullptr;

	int mCurrentFocusedMapIndex = -1;
};
