#include "Core/Camera.h"
#include "Core/EngineContext.h"
#include "Entity/Components/TransformComponent.h"
#include "Map/MapRegistry.h"

#include "LevelManager.h"

LevelManager::LevelManager()
{
	mLevelFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(LEVELS_FOLDER))
	{
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_LEVEL) != std::string::npos) continue;
		mLevelFileNames.push_back(_strdup(fileName.c_str()));
	}
}

const Level* LevelManager::GetCurrentLevel() const
{
	return mCurrentLevel.get();
}

void LevelManager::LoadLevel(const std::string& levelName, const EngineContext& context, MapRegistry& mapRegistry,
                             LuaSystem& luaSystem)
{
	if (mCurrentLevel != nullptr)
	{
		_UnloadLevel(context, mapRegistry, luaSystem);
	}

	// parse and serialize JSON
	std::string fullFileName = LEVELS_FOLDER;
	fullFileName.append(levelName);

	auto level = std::make_unique<Level>(fullFileName);

	// Create Map
	if (level->GetMapData() != nullptr)
	{
		mapRegistry.OpenMap(context, *level->GetMapData());
	}

	for (const auto& entity : level->GetEntityDefinitions())
	{
		const auto entityId = context.CreateGameObject(entity.name);
		auto& transformComponent = context.GetComponent<TransformComponent>(entityId);
		transformComponent.mPosition = entity.position;
	}

	for (const auto& script : level->GetScripts())
	{
		luaSystem.LoadScript(script.c_str(), context);
	}

	mCurrentLevel = std::move(level);
}

const std::vector<const char*>& LevelManager::GetLevelNames() const
{
	return mLevelFileNames;
}

void LevelManager::_UnloadLevel(const EngineContext& context, MapRegistry& mapRegistry,
                                LuaSystem& luaSystem) const
{
	luaSystem.QueueClose();
	mapRegistry.CloseMap(mCurrentLevel->GetMapData()->name);

	context.FlushEntities();
}
