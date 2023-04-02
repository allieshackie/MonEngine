#pragma once
#include "MapDescription.h"

class MapSystem
{
public:
	MapSystem() = default;
	~MapSystem() = default;

	void CreateMap(const std::string& mapPath);
	const std::vector<std::unique_ptr<MapDescription>>& GetAllMaps();
	std::unique_ptr<MapDescription>& GetCurrentMapDescription();

private:
	std::vector<std::unique_ptr<MapDescription>> mMaps;

	int mCurrentFocusedMapIndex = -1;
};
