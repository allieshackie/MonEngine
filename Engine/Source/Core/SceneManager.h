#pragma once
#include "Script/LuaBindable.h"
#include "World.h"

class Entity;
class EventPublisher;
class PrefabRegistry;
class RenderSystem;
class ResourceManager;

class SceneManager : public LuaBindable
{
public:
	SceneManager(PrefabRegistry& prefabRegistry, RenderSystem& renderSystem, ResourceManager& resourceManager, EventPublisher& eventPublisher);

	std::shared_ptr<World> GetCurrentWorld() const;
	void SetLuaSystem(std::weak_ptr<LuaSystem> luaSystem) { mLuaSystem = luaSystem; }

	void LoadScene(const std::string& sceneName);
	void SaveScene();
	void RestartScene();
	void CloseScene();
	const std::vector<const char*>& GetSceneNames() const;

	void BindMethods(lua_State* state) override;
	void BindInstanceGetter(lua_State* state) override;
	static constexpr char LuaName[] = "SceneManager";

private:
	std::shared_ptr<World> mCurrentWorld = nullptr;
	std::string mCurrentSceneName;

	std::vector<const char*> mSceneFileNames;

	RenderSystem& mRenderSystem;
	std::weak_ptr<LuaSystem> mLuaSystem;
	ResourceManager& mResourceManager;
	EventPublisher& mEventPublisher;
	PrefabRegistry& mPrefabRegistry;

	const char* EDITOR_SCENE = "editor";
};
