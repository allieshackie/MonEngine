#pragma once
#include <string>

class ConfigManager
{
public:
	ConfigManager() = default;
	~ConfigManager() = default;

	void LoadConfigFile(const std::string& projectDataPath, const std::string& configFile);

	const std::string& GetMapsFolderPath() const;
	const std::string& GetLevelsFolderPath() const;
	const std::string& GetEntitiesFolderPath() const;

	const std::string& GetModelsFolderPath() const;
	const std::string& GetTexturesFolderPath() const;
	const std::string& GetShadersFolderPath() const;

	const std::string& GetFontFolderPath() const;

private:
	static constexpr char MAPS_PATH_STRING[] = "maps_path";
	static constexpr char LEVELS_PATH_STRING[] = "levels_path";
	static constexpr char ENTITIES_PATH_STRING[] = "entities_path";
	static constexpr char MODELS_PATH_STRING[] = "models_path";
	static constexpr char TEXTURES_PATH_STRING[] = "textures_path";
	static constexpr char SHADERS_PATH_STRING[] = "shaders_path";
	static constexpr char FONT_PATH_STRING[] = "font_path";

	std::string mMapsFolderPath;
	std::string mLevelsFolderPath;
	std::string mEntitiesFolderPath;

	std::string mModelsFolderPath;
	std::string mTexturesFolderPath;
	std::string mShadersFolderPath;

	std::string mFontFolderPath;
};
