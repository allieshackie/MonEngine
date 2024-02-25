#pragma once

class Map;

struct MapData;

class MapRegistry
{
public:
	void OpenMap(const MapData& mapData);
	void OpenMap(const std::string& mapPath);
	void CloseMap(const std::string& mapId);

	const std::vector<std::shared_ptr<Map>>& GetAllMaps();
	Map& GetCurrentMap() const;

private:
	std::vector<std::shared_ptr<Map>> mMaps;

	std::shared_ptr<Map> mEmptyMap = nullptr;

	int mCurrentFocusedMapIndex = -1;
};
