#include "MapSystem.h"

void MapSystem::CreateMap(const std::string& mapPath)
{
	auto map = std::make_unique<MapDescription>(mapPath);
	mMaps.push_back(std::move(map));
	mCurrentFocusedMapIndex = static_cast<int>(mMaps.size()) - 1;
}

const std::vector<std::unique_ptr<MapDescription>>& MapSystem::GetAllMaps()
{
	return mMaps;
}

std::unique_ptr<MapDescription>& MapSystem::GetCurrentMapDescription()
{
	return mMaps[mCurrentFocusedMapIndex];
}
