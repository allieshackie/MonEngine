#pragma once
#include "Core/Camera.h"
#include "Scene.h"

class EngineContext;
class LuaSystem;
class MapRegistry;

class SceneManager
{
public:
	SceneManager(DescriptionFactory& descriptionFactory);

	// Returning the scene, which can possibly be nullptr
	const MonScene* GetCurrentScene() const;

	void LoadScene(const std::string& sceneName, const EngineContext& context, MapRegistry& mapRegistry,
	               LuaSystem& luaSystem);
	const std::vector<const char*>& GetSceneNames() const;

	Camera& GetCamera() const { return *mCurrentScene->mCamera; }

	Entity* CreateEntityFromTemplate(const char* templateName);
	Entity* CreateEntity() const;
	void RemoveEntity(const entt::entity id) const;

	void FlushEntities() const
	{
		for (const auto& entity : mCurrentScene->mRegistry.view<entt::entity>())
		{
			mCurrentScene->mRegistry.destroy(entity);
		}
	}

private:
	void _UnloadScene(const EngineContext& context, MapRegistry& mapRegistry,
	                  LuaSystem& luaSystem) const;

	void _ParseSceneJson(const std::string& sceneName, const EngineContext& context);

	std::shared_ptr<MonScene> mCurrentScene = nullptr;

	std::vector<const char*> mSceneFileNames;
	std::unique_ptr<EntityTemplateRegistry> mEntityTemplateRegistry;

	const char* EDITOR_SCENE = "editor";
};
