#include "Core/Camera.h"
#include "Core/EngineContext.h"
#include "Entity/Components/TransformComponent.h"
#include "Map/MapRegistry.h"

#include "LevelManager.h"

LevelManager::LevelManager(LuaSystem& luaSystem, MapRegistry& mapRegistry)
	: mLuaSystem(luaSystem), mMapRegistry(mapRegistry)
{
	mLevelFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(LEVELS_FOLDER))
	{
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_LEVEL) != std::string::npos) continue;
		mLevelFileNames.push_back(_strdup(fileName.c_str()));
	}
}

std::unique_ptr<Level>& LevelManager::GetCurrentLevel()
{
	return mCurrentLevel;
}

void LevelManager::LoadLevel(const std::string& levelName, const EngineContext& context)
{
	if (mCurrentLevel != nullptr)
	{
		_UnloadLevel(context);
	}

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

	for (const auto& script : level->GetScripts())
	{
		mLuaSystem.LoadScript(script.c_str(), context);
	}

	mCurrentLevel = std::move(level);
}

const std::vector<const char*>& LevelManager::GetLevelNames() const
{
	return mLevelFileNames;
}

void LevelManager::_UnloadLevel(const EngineContext& context) const
{
	mLuaSystem.QueueClose();

	if (const auto& mapData = mCurrentLevel->GetMapData())
	{
		mMapRegistry.CloseMap(mapData->name);
	}

	context.FlushEntities();
}
