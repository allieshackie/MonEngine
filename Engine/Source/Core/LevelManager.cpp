#include "Core/Camera.h"
#include "Core/EngineContext.h"
#include "Entity/Components/TransformComponent.h"
#include "Map/MapRegistry.h"

#include "LevelManager.h"

LevelManager::LevelManager(MapRegistry& mapRegistry)
	: mMapRegistry(mapRegistry)
{
}

std::unique_ptr<Level>& LevelManager::GetLevel(const std::string& levelName)
{
	const auto it = mLevels.find(levelName);
	assert(it != mLevels.end());
	return it->second;
}

std::unique_ptr<Level>& LevelManager::GetCurrentLevel()
{
	if (mLevels.empty())
	{
		return mEmptyLevel;
	}
	// return last element 
	return std::prev(mLevels.end())->second;
}

void LevelManager::LoadLevel(const std::string& levelName, const EngineContext& context)
{
	// parse and serialize JSON
	std::string fullFileName = LEVELS_FOLDER;
	fullFileName.append(levelName);

	auto level = std::make_unique<Level>(fullFileName);

	// Create Map
	if (const auto& mapData = level->GetMapData())
	{
		mMapRegistry.OpenMap(mapData->name, mapData->position, mapData->rotation, mapData->tileSize);
	}

	for (const auto& entity : level->GetEntityDefinitions())
	{
		const auto entityId = context.CreateGameObject(entity.name);
		auto& transformComponent = context.GetComponent<TransformComponent>(entityId);
		transformComponent.mPosition = entity.position;
	}

	mLevels.insert(std::make_pair(levelName, std::move(level)));
}
