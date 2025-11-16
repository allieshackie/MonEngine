#pragma once
#include "Entity/PrefabRegistry.h"
#include "World.h"

class DescriptionFactory;
class Entity;
class LuaSystem;
class MapRegistry;

class SceneManager
{
public:
	SceneManager(DescriptionFactory& descriptionFactory, MapRegistry& mapRegistry, LuaSystem& luaSystem);

	std::shared_ptr<World> GetCurrentWorld() const;

	void LoadScene(const std::string& sceneName) const;
	const std::vector<const char*>& GetSceneNames() const;

private:
	std::shared_ptr<World> mCurrentWorld = nullptr;

	std::vector<const char*> mSceneFileNames;
	std::unique_ptr<PrefabRegistry> mPrefabRegistry = nullptr;

	MapRegistry& mMapRegistry;
	LuaSystem& mLuaSystem;

	const char* EDITOR_SCENE = "editor";
};
