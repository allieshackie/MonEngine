#pragma once
#include "MapDescription.h"

class MapSystem
{
public:
	MapSystem() = default;
	~MapSystem() = default;

	void CreateMap(const std::string& mapPath);
	const std::vector<std::shared_ptr<MapDescription>>& GetAllMaps();
	std::shared_ptr<MapDescription> GetCurrentMapDescription();

	void CloseCurrentMap();

private:
	std::vector<std::shared_ptr<MapDescription>> mMaps;

	int mCurrentFocusedMapIndex = -1;
};
