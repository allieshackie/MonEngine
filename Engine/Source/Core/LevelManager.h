#pragma once
#include "Level.h"

class EngineContext;
class MapRegistry;

class LevelManager
{
public:
	LevelManager(MapRegistry& mapRegistry);

	std::unique_ptr<Level>& GetLevel(const std::string& levelName);
	std::unique_ptr<Level>& GetCurrentLevel();

	void LoadLevel(const std::string& levelName, const EngineContext& context);

private:
	std::map<std::string, std::unique_ptr<Level>> mLevels;
	std::unique_ptr<Level> mEmptyLevel{nullptr};

	MapRegistry& mMapRegistry;
};
