#include "Map.h"

#include "MapRegistry.h"

void MapRegistry::OpenMap(const MapData& mapData)
{
	mMaps.emplace_back(std::make_unique<Map>(mapData));
	mCurrentFocusedMapIndex = static_cast<int>(mMaps.size()) - 1;
}

void MapRegistry::OpenMap(const std::string& mapPath)
{
	mMaps.push_back(std::make_unique<Map>(mapPath));
	mCurrentFocusedMapIndex = static_cast<int>(mMaps.size()) - 1;
}

const std::vector<std::shared_ptr<Map>>& MapRegistry::GetAllMaps()
{
	return mMaps;
}

Map& MapRegistry::GetCurrentMap() const
{
	if (mCurrentFocusedMapIndex == -1) return *mEmptyMap;
	return *mMaps[mCurrentFocusedMapIndex];
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
