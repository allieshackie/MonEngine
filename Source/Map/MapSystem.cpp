#include "MapSystem.h"

void MapSystem::CreateMap(const std::string& mapPath)
{
	auto map = std::make_unique<MapDescription>(mapPath);
	mMaps.push_back(std::move(map));
	mCurrentFocusedMapIndex = static_cast<int>(mMaps.size()) - 1;
}

const std::vector<std::shared_ptr<MapDescription>>& MapSystem::GetAllMaps()
{
	return mMaps;
}

std::shared_ptr<MapDescription> MapSystem::GetCurrentMapDescription()
{
	if (mCurrentFocusedMapIndex == -1) return nullptr;
	return mMaps[mCurrentFocusedMapIndex];
}

void MapSystem::CloseCurrentMap()
{
	mMaps.erase(mMaps.end() - 1);
	mCurrentFocusedMapIndex = -1;
}
