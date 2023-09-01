#include <nlohmann/json.hpp>

#include "ConfigManager.h"

void ConfigManager::LoadConfigFile(const std::string& projectDataPath, const std::string& configFile)
{
	// parse and serialize JSON
	std::string fullFileName = projectDataPath;
	fullFileName += configFile;

	std::ifstream ifs(fullFileName.c_str());

	auto json = nlohmann::json::parse(ifs, nullptr, false, true);

	mMapsFolderPath = projectDataPath;
	mMapsFolderPath.append(json[MAPS_PATH_STRING]);

	mLevelsFolderPath = projectDataPath;
	mLevelsFolderPath.append(json[LEVELS_PATH_STRING]);

	mEntitiesFolderPath = projectDataPath;
	mEntitiesFolderPath.append(json[ENTITIES_PATH_STRING]);

	mModelsFolderPath = projectDataPath;
	mModelsFolderPath.append(json[MODELS_PATH_STRING]);

	mTexturesFolderPath = projectDataPath;
	mTexturesFolderPath.append(json[TEXTURES_PATH_STRING]);

	mShadersFolderPath = projectDataPath;
	mShadersFolderPath.append(json[SHADERS_PATH_STRING]);

	mFontFolderPath = projectDataPath;
	mFontFolderPath.append(json[FONT_PATH_STRING]);
}

const std::string& ConfigManager::GetMapsFolderPath() const
{
	return mMapsFolderPath;
}

const std::string& ConfigManager::GetLevelsFolderPath() const
{
	return mLevelsFolderPath;
}

const std::string& ConfigManager::GetEntitiesFolderPath() const
{
	return mEntitiesFolderPath;
}

const std::string& ConfigManager::GetModelsFolderPath() const
{
	return mModelsFolderPath;
}

const std::string& ConfigManager::GetTexturesFolderPath() const
{
	return mTexturesFolderPath;
}

const std::string& ConfigManager::GetShadersFolderPath() const
{
	return mShadersFolderPath;
}

const std::string& ConfigManager::GetFontFolderPath() const
{
	return mFontFolderPath;
}
