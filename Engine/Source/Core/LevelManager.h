#pragma once
#include "Core/Camera.h"
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

	Camera& GetCamera() const { return *mCamera; }

private:
	void _UnloadLevel(const EngineContext& context, MapRegistry& mapRegistry,
	                  LuaSystem& luaSystem) const;

	void _ParseLevelJson(const std::string& levelName);

	std::unique_ptr<Level> mCurrentLevel = nullptr;
	std::unique_ptr<Camera> mCamera = nullptr;

	std::vector<const char*> mLevelFileNames;

	const char* EDITOR_LEVEL = "editor";

	static constexpr char CAMERA_STRING[] = "camera";
	static constexpr char POSITION_STRING[] = "position";
	static constexpr char FRONT_STRING[] = "front";
	static constexpr char UP_STRING[] = "up";

	static constexpr char MAPS_STRING[] = "maps";
	static constexpr char NAME_STRING[] = "name";
	static constexpr char ROTATION_STRING[] = "rotation";
	static constexpr char SIZE_STRING[] = "size";
	static constexpr char TILE_SIZE_STRING[] = "tile_size";
	static constexpr char HAS_DIMENSION_STRING[] = "has_dimension";

	static constexpr char ENTITIES_STRING[] = "entities";
	static constexpr char SCRIPTS_STRING[] = "scripts";

	static constexpr char LIGHTS_STRING[] = "lights";
	static constexpr char COLOR_STRING[] = "color";
	static constexpr char INTENSITY_STRING[] = "intensity";
};
