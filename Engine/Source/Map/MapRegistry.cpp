#include <glm/vec2.hpp>
#include "Map.h"

#include "MapRegistry.h"

void MapRegistry::OpenMap(const std::string& mapPath, glm::vec3 pos, glm::vec3 rotation, float tileSize)
{
	std::string fullMapPath = MAPS_FOLDER;
	fullMapPath.append(mapPath);

	auto map = std::make_shared<Map>(fullMapPath, pos, rotation, tileSize);
	mMaps.push_back(std::move(map));
}

void MapRegistry::OpenMap(const std::string& mapPath)
{
	std::string fullMapPath = MAPS_FOLDER;
	fullMapPath.append(mapPath);

	auto map = std::make_shared<Map>(fullMapPath);
	mMaps.push_back(std::move(map));
	mCurrentFocusedMapIndex = static_cast<int>(mMaps.size()) - 1;
}

const std::vector<std::shared_ptr<Map>>& MapRegistry::GetAllMaps()
{
	return mMaps;
}

std::shared_ptr<Map> MapRegistry::GetCurrentMap()
{
	if (mCurrentFocusedMapIndex == -1) return nullptr;
	return mMaps[mCurrentFocusedMapIndex];
}

void MapRegistry::CloseMap(const std::string& mapId)
{
	const auto it = std::find_if(mMaps.begin(), mMaps.end(), [&mapId](const std::shared_ptr<Map>& map)
	{
		return map->GetMapId() == mapId;
	});

	if (it != mMaps.end())
	{
		mMaps.erase(it);
		mCurrentFocusedMapIndex = -1;
	}
}
