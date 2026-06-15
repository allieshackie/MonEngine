#pragma once

#include "Core/Camera.h"
#include "EventListener.h"
#include "Graphics/Core/TerrainMesh.h"

class LuaSystem;
class MonScene;
class PrefabRegistry;
class RenderSystem;
class ResourceManager;

class World
{
public:
	World();

	void Close();

	void Init(MonScene* scene, PrefabRegistry& prefabRegistry, RenderSystem& renderSystem, ResourceManager& resourceManager, std::weak_ptr<LuaSystem> luaSystem);
	Camera& GetCamera() const { return *mCamera; }
	entt::registry& GetRegistry() { return mRegistry; }
	Entity* GetEntityForId(entt::entity id);
	const std::unique_ptr<TerrainMesh>& GetTerrain() const { return mTerrain; }

	void CreateCamera(const MonScene* scene);
	Entity& CreateEntityFromTemplate(const char* templateName, PrefabRegistry& prefabRegistry);
	Entity& CreateEntity();
	void RemoveEntity(const entt::entity id);
	void FlushEntities();
	void DisconnectAll();

	template <typename Component>
	SubscriptionHandle ConnectOnConstruct(EventFunc& handler);
	template <typename Component>
	SubscriptionHandle ConnectOnDestroy(EventFunc& handler);

	template <typename Component>
	void RegisterComponentLifecycle();
	template <typename Component>
	void OnComponentDestroyed(entt::entity entity);

private:
	std::unique_ptr<Camera> mCamera = nullptr;
	entt::registry mRegistry;
	std::unordered_map<std::string, int> mTypeCounters;
	std::unordered_map<entt::entity, Entity*> mEntityMap;
	std::unordered_map<std::string, entt::entity> mEntityNameIdMap;
	std::unique_ptr<EventPublisher> mEventPublisher;
	std::unique_ptr<TerrainMesh> mTerrain;

	std::vector<std::pair<std::string, SubscriptionHandle>> mSubscriptions;
};

template <typename Component>
SubscriptionHandle World::ConnectOnConstruct(EventFunc& handler)
{
	SubscriptionHandle handle = mEventPublisher->AddListener<Component>("on_construct", handler);
	mSubscriptions.emplace_back("on_construct", handle);

	return handle;
}

template <typename Component>
SubscriptionHandle World::ConnectOnDestroy(EventFunc& handler)
{
	SubscriptionHandle handle = mEventPublisher->AddListener<Component>("on_destroy", handler);
	mSubscriptions.emplace_back("on_destroy", handle);

	return handle;
}

template <typename Component>
void World::RegisterComponentLifecycle()
{
	mRegistry.on_destroy<Component>().connect<&World::OnComponentDestroyed<Component>>(this);
}

template <typename Component>
void World::OnComponentDestroyed(entt::entity entity)
{
	auto it = mEntityMap.find(entity);
	if (it != mEntityMap.end())
	{
		mEventPublisher->Notify<Component>("on_destroy", it->second);
	}
}
