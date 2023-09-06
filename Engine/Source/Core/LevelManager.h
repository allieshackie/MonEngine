#pragma once
#include "Level.h"

class MapSystem;

class LevelManager
{
public:
	LevelManager(std::string levelsFolderPath, MapSystem& mapSystem);
	~LevelManager() = default;

	std::unique_ptr<Level>& GetLevel(const std::string& levelName);
	std::unique_ptr<Level>& GetCurrentLevel();

	void LoadLevel(const std::string& levelName);

private:
	std::map<std::string, std::unique_ptr<Level>> mLevels;
	std::unique_ptr<Level> mEmptyLevel{nullptr};

	std::string mLevelsFolderPath;

	MapSystem& mMapSystem;
};
