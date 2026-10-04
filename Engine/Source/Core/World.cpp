#include "Core/Scene.h"
#include "Entity/Entity.h"
#include "Entity/Components/AnimationComponent.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/InteractComponent.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/PlayerComponent.h"
#include "Entity/Components/ScriptComponent.h"
#include "Entity/Components/SpriteComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/TriggerVolumeComponent.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/RenderSystem.h"
#include "Script/LuaSystem.h"

#include "World.h"

World::World()
{
	mEventPublisher = std::make_unique<EventPublisher>();
	RegisterComponentLifecycle<AnimationComponent>();
	RegisterComponentLifecycle<CollisionComponent>();
	RegisterComponentLifecycle<LightComponent>();
	RegisterComponentLifecycle<MapComponent>();
	RegisterComponentLifecycle<ModelComponent>();
	RegisterComponentLifecycle<PhysicsComponent>();
	RegisterComponentLifecycle<PlayerComponent>();
	RegisterComponentLifecycle<ScriptComponent>();
	RegisterComponentLifecycle<SpriteComponent>();
	RegisterComponentLifecycle<TransformComponent>();
	RegisterComponentLifecycle<TriggerVolumeComponent>();
}

World::~World()
{
	Close();
}

void World::Update()
{
	if (mCamera)
	{
		mCamera->Update();
	}
}

void World::FlushEvents()
{
	mEventPublisher->Flush();

	auto entitiesToDestroy = std::move(mEntitiesToDestroy);
	mEntitiesToDestroy.clear();
	for (const auto id : entitiesToDestroy)
	{
		if (!mRegistry.valid(id))
		{
			continue;
		}

		mRegistry.destroy(id);
		mEventPublisher->Flush();
		mEntityMap.erase(id);
		for (auto nameIt = mEntityNameIdMap.begin(); nameIt != mEntityNameIdMap.end();)
		{
			if (nameIt->second == id)
			{
				nameIt = mEntityNameIdMap.erase(nameIt);
			}
			else
			{
				++nameIt;
			}
		}
	}
}

SubscriptionHandle World::ConnectOnPhysicsEvent(PhysicsEventType type, PhysicsEventFunc& handler)
{
	SubscriptionHandle handle = mEventPublisher->AddListener(type, handler);
	mPhysicsSubscriptions.emplace_back(type, handle);

	return handle;
}

void World::NotifyPhysicsEvent(PhysicsEventType type, entt::entity entityA, entt::entity entityB)
{
	mEventPublisher->Notify(type, entityA, entityB);
}

void World::Init(const MonScene& scene, PrefabRegistry& prefabRegistry, RenderSystem& renderSystem, ResourceManager& resourceManager, std::weak_ptr<LuaSystem> luaSystem)
{
	CreateCamera(scene);

	for (const auto& entity : scene.GetEntityOverrides())
	{
		auto& gameObj = CreateEntityFromTemplate(entity.mPrefab.c_str(), prefabRegistry, entity.mOverrides);
	}

	if (const auto luaPtr = luaSystem.lock())
	{
		for (const auto& script : scene.GetScripts())
		{
			luaPtr->LoadScript(script.c_str());
		}
	}
}

Entity& World::CreateEntityFromTemplate(const char* templateName, PrefabRegistry& prefabRegistry, const std::vector<SerializedComponent>& overrides)
{
	auto& components = prefabRegistry.GetPrefabComponents(templateName);
	auto id = mRegistry.create();
	std::string name = templateName + std::to_string(mEntityMap.size());
	mEntityMap[id] = std::make_unique<Entity>(id, mRegistry, *mEventPublisher, name, templateName);
	mEntityNameIdMap[std::to_string(static_cast<uint32_t>(id))] = id;

	for (auto& component : components)
	{
		auto& componentKey = component.key;
		auto updateJson = component.json;
		auto it = std::find_if(overrides.begin(), overrides.end(), [componentKey](const SerializedComponent& u) {return u.key == componentKey;});
		if (it != overrides.end())
		{
			updateJson.update(it->json);
		}
		component.saveLoader.loader(mEntityMap[id].get(), updateJson);
	}

	return *mEntityMap[id];
}

Entity& World::CreateEntity()
{
	auto id = mRegistry.create();
	mEntityMap[id] = std::make_unique<Entity>(id, mRegistry, *mEventPublisher);;

	return *mEntityMap[id];
}

void World::RemoveEntity(const entt::entity id)
{
	if (mRegistry.valid(id) && std::find(mEntitiesToDestroy.begin(), mEntitiesToDestroy.end(), id) == mEntitiesToDestroy.end())
	{
		mEntitiesToDestroy.push_back(id);
	}
}

void World::Close()
{
	mIsClosing = true;
	mCamera = nullptr;
	FlushEntities();
	DisconnectAll();
}

void World::DisconnectAll()
{
	for (const auto& [eventType, handle] : mSubscriptions)
	{
		mEventPublisher->RemoveListener(eventType, handle);
	}
	mSubscriptions.clear();

	for (const auto& [eventType, handle] : mPhysicsSubscriptions)
	{
		mEventPublisher->RemoveListener(eventType, handle);
	}
	mPhysicsSubscriptions.clear();
}

void World::FlushEntities()
{
	mRegistry.clear();
	mEntityMap.clear();
	mEntityNameIdMap.clear();
	mEntitiesToDestroy.clear();
}

Entity* World::GetEntityForId(entt::entity id)
{
	if (const auto it = mEntityMap.find(id); it != mEntityMap.end())
	{
		return it->second.get();
	}
	return nullptr;
}

void World::CreateCamera(const MonScene& scene)
{
	mCamera = std::make_unique<Camera>(this, scene.GetCameraData());
}
