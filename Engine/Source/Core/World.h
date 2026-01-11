#pragma once

#include "Core/Camera.h"
#include "EventListener.h"
#include "Graphics/Core/TerrainMesh.h"

class LuaSystem;
class MapRegistry;
class MonScene;
class PrefabRegistry;
class ResourceManager;

class World
{
public:
	World();

	void Close();

	void Init(MonScene* scene, PrefabRegistry& prefabRegistry, const MapRegistry& mapRegistry, ResourceManager& resourceManager,
	          std::weak_ptr<LuaSystem> luaSystem);
	Camera& GetCamera() const { return *mCamera; }
	entt::registry& GetRegistry() { return mRegistry; }
	Entity* GetEntityForId(entt::entity id);
	const std::unique_ptr<TerrainMesh>& GetTerrain() const { return mTerrain; }

	void CreateCamera(const MonScene* scene);
	Entity& CreateEntityFromTemplate(const char* templateName, PrefabRegistry& prefabRegistry);
	Entity& CreateEntity();
	void RemoveEntity(const entt::entity id);
	void FlushEntities();

	template <typename Component>
	void ConnectOnConstruct(EventFunc& handler) const;
	template <typename Component>
	void ConnectOnDestroy(EventFunc& handler) const;

private:
	std::unique_ptr<Camera> mCamera = nullptr;
	entt::registry mRegistry;
	std::unordered_map<std::string, int> mTypeCounters;
	std::unordered_map<entt::entity, Entity*> mEntityMap;
	std::unordered_map<std::string, entt::entity> mEntityNameIdMap;
	std::unique_ptr<EventPublisher> mEventPublisher;
	std::unique_ptr<TerrainMesh> mTerrain;
};

template <typename Component>
void World::ConnectOnConstruct(EventFunc& handler) const
{
	mEventPublisher->AddListener<Component>("on_construct", handler);
}

template <typename Component>
void World::ConnectOnDestroy(EventFunc& handler) const
{
	mEventPublisher->AddListener<Component>("on_destroy", handler);
}
