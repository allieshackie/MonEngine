#pragma once
#include "Entity/PrefabRegistry.h"
#include "Script/LuaBindable.h"
#include "World.h"

class DescriptionFactory;
class Entity;
class RenderSystem;
class ResourceManager;

class SceneManager : public LuaBindable
{
public:
	SceneManager(DescriptionFactory& descriptionFactory, RenderSystem& renderSystem, ResourceManager& resourceManager);

	std::shared_ptr<World> GetCurrentWorld() const;
	void SetLuaSystem(std::weak_ptr<LuaSystem> luaSystem) { mLuaSystem = luaSystem; }

	void LoadScene(const std::string& sceneName);
	void RestartScene();
	const std::vector<const char*>& GetSceneNames() const;

	void BindMethods(lua_State* state) override;
	void BindInstanceGetter(lua_State* state) override;
	static constexpr char LuaName[] = "SceneManager";

private:
	std::shared_ptr<World> mCurrentWorld = nullptr;
	std::string mCurrentSceneName;

	std::vector<const char*> mSceneFileNames;
	std::unique_ptr<PrefabRegistry> mPrefabRegistry = nullptr;

	RenderSystem& mRenderSystem;
	std::weak_ptr<LuaSystem> mLuaSystem;
	ResourceManager& mResourceManager;

	const char* EDITOR_SCENE = "editor";
};
