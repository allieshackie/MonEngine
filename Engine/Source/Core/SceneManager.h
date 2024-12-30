#pragma once
#include "Core/Camera.h"
#include "Entity/EntityTemplateRegistry.h"
#include "Scene.h"

class DescriptionFactory;
class EngineContext;
class LuaSystem;
class MapRegistry;

class Entity;

class SceneManager
{
public:
	SceneManager(DescriptionFactory& descriptionFactory);

	// Returning the scene, which can possibly be nullptr
	MonScene* GetCurrentScene() const;

	void LoadScene(const std::string& sceneName, const EngineContext& context, MapRegistry& mapRegistry,
	               LuaSystem& luaSystem);
	const std::vector<const char*>& GetSceneNames() const;

	Camera& GetCamera() const { return mCurrentScene->GetCamera(); }

	Entity& CreateEntityFromTemplate(const char* templateName) const;
	Entity& CreateEntity() const;
	void RemoveEntity(const entt::entity id) const;
	void FlushEntities() const;

	template <typename Component>
	void ConnectOnConstruct(EventFunc& handler) const;
	template <typename Component>
	void ConnectOnDestroy(EventFunc& handler) const;

private:
	void _UnloadScene(MapRegistry& mapRegistry,
	                  LuaSystem& luaSystem) const;

	void _ParseSceneJson(const std::string& sceneName);

	std::unique_ptr<MonScene> mCurrentScene = nullptr;
	std::unique_ptr<EventPublisher> mEventPublisher;

	std::vector<const char*> mSceneFileNames;
	std::unique_ptr<EntityTemplateRegistry> mEntityTemplateRegistry = nullptr;

	const char* EDITOR_SCENE = "editor";
};


template <typename Component>
void SceneManager::ConnectOnConstruct(EventFunc& handler) const
{
	mEventPublisher->AddListener<Component>("on_construct", handler);
}

template <typename Component>
void SceneManager::ConnectOnDestroy(EventFunc& handler) const
{
	mEventPublisher->AddListener<Component>("on_destroy", handler);
}
