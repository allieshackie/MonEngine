#include "Core/Scene.h"
#include "Entity/Entity.h"
#include "Entity/PrefabRegistry.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Descriptions/DescriptionBase.h"
#include "Map/MapRegistry.h"
#include "Scripting/LuaSystem.h"

#include "World.h"

World::World()
{
	mEventPublisher = std::make_unique<EventPublisher>();
}

void World::Close()
{
	//luaSystem.QueueClose();
	FlushEntities();
}

void World::Init(MonScene* scene, PrefabRegistry& prefabRegistry, const MapRegistry& mapRegistry,
                 LuaSystem& luaSystem)
{
	// Create Map
	if (!scene->GetMapData().mName.empty())
	{
		mapRegistry.OpenMap(this, scene->GetMapData());
	}

	CreateCamera(scene);

	for (const auto& entity : scene->GetEntityDefinitions())
	{
		auto& gameObj = CreateEntityFromTemplate(entity.mName.c_str(), prefabRegistry);
		auto& transformComponent = gameObj.GetComponent<TransformComponent>();
		transformComponent.mPosition = entity.mPosition;
		if (const auto collider = gameObj.TryGetComponent<CollisionComponent>(); collider != nullptr)
		{
			collider->mInitialized = true;
		}
	}

	for (const auto& script : scene->GetScripts())
	{
		luaSystem.LoadScript(script.c_str());
	}
}

Entity& World::CreateEntityFromTemplate(const char* templateName, PrefabRegistry& prefabRegistry)
{
	const auto& descriptions = prefabRegistry.GetPrefabsDescriptions(templateName);
	auto id = mRegistry.create();
	const auto entity = new Entity(id, mRegistry, *mEventPublisher, templateName);
	mEntityMap[id] = entity;
	mEntityNameIdMap[std::to_string(static_cast<uint32_t>(id))] = id;

	for (const auto& description : descriptions)
	{
		description->ApplyToEntity(entity, mRegistry);
	}

	return *mEntityMap[id];
}

Entity& World::CreateEntity()
{
	auto id = mRegistry.create();
	const auto entity = new Entity(id, mRegistry, *mEventPublisher);
	mEntityMap[id] = entity;

	return *mEntityMap[id];
}

void World::RemoveEntity(const entt::entity id)
{
	mRegistry.destroy(id);
}

void World::FlushEntities()
{
	for (const auto& entity : mRegistry.view<entt::entity>())
	{
		mRegistry.destroy(entity);
	}
}

Entity* World::GetEntityForId(entt::entity id)
{
	if (const auto it = mEntityMap.find(id); it != mEntityMap.end())
	{
		return it->second;
	}
	return nullptr;
}

void World::CreateCamera(const MonScene* scene)
{
	mCamera = std::make_unique<Camera>(this,
	                                   scene->GetCameraData().mCameraPos,
	                                   scene->GetCameraData().mCameraFront,
	                                   scene->GetCameraData().mCameraUp,
	                                   scene->GetCameraData().mFollowCam);
}
