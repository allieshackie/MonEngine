#include "LevelManager.h"

LevelManager::LevelManager(std::string levelsFolderPath, InputManager& inputManager, MapSystem& mapSystem)
	: mInputManager(inputManager), mLevelsFolderPath(std::move(levelsFolderPath)), mMapSystem(mapSystem)
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

void LevelManager::LoadLevel(const std::string& levelName)
{
	// parse and serialize JSON
	std::string fullFileName = mLevelsFolderPath;
	fullFileName.append(levelName);

	auto level = std::make_unique<Level>(fullFileName, mMapSystem, mInputManager);
	mLevels.insert(std::make_pair(levelName, std::move(level)));
}
