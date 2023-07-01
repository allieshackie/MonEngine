#include "Map.h"

#include "MapSystem.h"

void MapSystem::CreateMap(const std::string& mapPath, glm::vec3 pos, glm::vec3 rotation, float tileSize)
{
	auto map = std::make_shared<Map>(mapPath, pos, rotation, tileSize);
	mOnCreateCallback(map);
	mMaps.push_back(std::move(map));
}

void MapSystem::CreateMapEditMode(const std::string& mapPath)
{
	auto map = std::make_shared<Map>(mapPath, mOnUpdateCallback);
	mOnCreateCallback(map);
	mMaps.push_back(std::move(map));
	mCurrentFocusedMapIndex = static_cast<int>(mMaps.size()) - 1;
}

const std::vector<std::shared_ptr<Map>>& MapSystem::GetAllMaps()
{
	return mMaps;
}

std::shared_ptr<Map> MapSystem::GetCurrentMap()
{
	if (mCurrentFocusedMapIndex == -1) return nullptr;
	return mMaps[mCurrentFocusedMapIndex];
}

void MapSystem::CloseCurrentMap()
{
	mMaps.erase(mMaps.end() - 1);
	mCurrentFocusedMapIndex = -1;
}

void MapSystem::RegisterOnCreateCallback(std::function<void(std::shared_ptr<Map>&)> cb)
{
	mOnCreateCallback = std::move(cb);
}

void MapSystem::RegisterOnUpdateCallback(std::function<void()> cb)
{
	mOnUpdateCallback = std::move(cb);
}
