#include "LevelManager.h"

LevelManager::LevelManager(MapSystem& mapSystem, InputManager& inputManager)
	: mInputManager(inputManager), mMapSystem(mapSystem)
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
	assert(!mLevels.empty());
	return mLevels.begin()->second;
}

void LevelManager::LoadLevel(const std::string& levelName)
{
	auto level = std::make_unique<Level>(levelName, mMapSystem, mInputManager);
	mLevels.insert(std::make_pair(levelName, std::move(level)));
}
