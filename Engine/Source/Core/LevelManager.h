#pragma once
#include "Level.h"

class InputManager;
class MapSystem;

class LevelManager
{
public:
	LevelManager(MapSystem& mapSystem, InputManager& inputManager);
	~LevelManager() = default;

	std::unique_ptr<Level>& GetLevel(const std::string& levelName);
	std::unique_ptr<Level>& GetCurrentLevel();

	void LoadLevel(const std::string& levelName);


private:
	std::map<std::string, std::unique_ptr<Level>> mLevels;

	InputManager& mInputManager;
	MapSystem& mMapSystem;
};
