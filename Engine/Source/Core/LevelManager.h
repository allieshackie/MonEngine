#pragma once
#include "Level.h"

class EngineContext;
class LuaSystem;
class MapRegistry;

class LevelManager
{
public:
	LevelManager();

	// Returning the level, which can possibly be nullptr
	const Level* GetCurrentLevel() const;

	void LoadLevel(const std::string& levelName, const EngineContext& context, MapRegistry& mapRegistry,
	               LuaSystem& luaSystem);
	const std::vector<const char*>& GetLevelNames() const;

private:
	void _UnloadLevel(const EngineContext& context, MapRegistry& mapRegistry,
	                  LuaSystem& luaSystem) const;

	std::unique_ptr<Level> mCurrentLevel;
	std::vector<const char*> mLevelFileNames;
	std::unique_ptr<Level> mEmptyLevel{nullptr};

	const char* EDITOR_LEVEL = "editor";
};
