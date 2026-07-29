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
	bool IsClosing() const { return mIsClosing; }

	void Init(const MonScene& scene, PrefabRegistry& prefabRegistry, RenderSystem& renderSystem, ResourceManager& resourceManager, std::weak_ptr<LuaSystem> luaSystem);
	Camera& GetCamera() const { return *mCamera; }
	entt::registry& GetRegistry() { return mRegistry; }
	Entity* GetEntityForId(entt::entity id);
	const std::unique_ptr<TerrainMesh>& GetTerrain() const { return mTerrain; }

	void CreateCamera(const MonScene& scene);
	Entity& CreateEntityFromTemplate(const char* templateName, PrefabRegistry& prefabRegistry);
	Entity& CreateEntity();
	void RemoveEntity(const entt::entity id);
	void FlushEntities();
	void DisconnectAll();

	void Update();
	void FlushEvents();

	template <typename Component>
	SubscriptionHandle ConnectOnConstruct(EntityEventFunc& handler);
	template <typename Component>
	SubscriptionHandle ConnectOnDestroy(EntityEventFunc& handler);

	template <typename Component>
	void RegisterComponentLifecycle();
	template <typename Component>
	void OnComponentDestroyed(entt::entity entity);

	SubscriptionHandle ConnectOnPhysicsEvent(PhysicsEventType type, PhysicsEventFunc& handler);
	void NotifyPhysicsEvent(PhysicsEventType type, entt::entity entityA, entt::entity entityB);

private:
	std::unique_ptr<Camera> mCamera;
	entt::registry mRegistry;
	std::unordered_map<std::string, int> mTypeCounters;
	std::unordered_map<entt::entity, std::unique_ptr<Entity>> mEntityMap;
	std::unordered_map<std::string, entt::entity> mEntityNameIdMap;
	std::unique_ptr<EventPublisher> mEventPublisher;
	std::unique_ptr<TerrainMesh> mTerrain;

	std::vector<std::pair<std::string, SubscriptionHandle>> mSubscriptions;
	std::vector<std::pair<PhysicsEventType, SubscriptionHandle>> mPhysicsSubscriptions;

	bool mIsClosing = false;
};

template <typename Component>
SubscriptionHandle World::ConnectOnConstruct(EntityEventFunc& handler)
{
	SubscriptionHandle handle = mEventPublisher->AddListener("on_construct", std::type_index(typeid(Component)), handler);
	mSubscriptions.emplace_back("on_construct", handle);

	return handle;
}

template <typename Component>
SubscriptionHandle World::ConnectOnDestroy(EntityEventFunc& handler)
{
	SubscriptionHandle handle = mEventPublisher->AddListener("on_destroy", std::type_index(typeid(Component)), handler);
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
	mEventPublisher->Notify("on_destroy", std::type_index(typeid(Component)), entity);
}
