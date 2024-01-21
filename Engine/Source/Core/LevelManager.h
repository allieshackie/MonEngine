#pragma once
#include "Level.h"

class EngineContext;
class LuaSystem;
class MapRegistry;

class LevelManager
{
public:
	LevelManager(LuaSystem& luaSystem, MapRegistry& mapRegistry);

	std::unique_ptr<Level>& GetLevel(const std::string& levelName);
	std::unique_ptr<Level>& GetCurrentLevel();

	void LoadLevel(const std::string& levelName, const EngineContext& context);
	const std::vector<const char*>& GetLevelNames() const;

private:
	std::map<std::string, std::unique_ptr<Level>> mLevels;
	std::vector<const char*> mLevelFileNames;
	std::unique_ptr<Level> mEmptyLevel{nullptr};

	const char* EDITOR_LEVEL = "editor";

	LuaSystem& mLuaSystem;
	MapRegistry& mMapRegistry;
};
