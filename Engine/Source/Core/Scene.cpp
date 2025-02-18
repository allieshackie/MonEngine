#include "Entity/Descriptions/DescriptionFactory.h"
#include "Entity/EntityTemplateRegistry.h"

#include "Scene.h"

Entity& MonScene::CreateEntityFromTemplate(const char* templateName, EntityTemplateRegistry& templateRegistry)
{
	const auto& descriptions = templateRegistry.GetEntityTemplateDescriptions(templateName);
	auto id = mRegistry.create();
	const auto entity = new Entity(id, mRegistry, mEventPublisher, templateName);
	mEntityMap[id] = entity;
	mEntityNameIdMap[std::to_string(static_cast<uint32_t>(id))] = id;

	for (const auto& description : descriptions)
	{
		description->ApplyToEntity(entity, mRegistry);
	}

	return *mEntityMap[id];
}

Entity& MonScene::CreateEntity()
{
	auto id = mRegistry.create();
	const auto entity = new Entity(id, mRegistry, mEventPublisher);
	mEntityMap[id] = entity;

	return *mEntityMap[id];
}

void MonScene::RemoveEntity(const entt::entity id)
{
	mRegistry.destroy(id);
}

void MonScene::FlushEntities()
{
	for (const auto& entity : mRegistry.view<entt::entity>())
	{
		mRegistry.destroy(entity);
	}
}

Entity* MonScene::GetEntityForId(entt::entity id)
{
	if (const auto it = mEntityMap.find(id); it != mEntityMap.end())
	{
		return it->second;
	}
	return nullptr;
}

void MonScene::CreateCamera(const SceneManager& sceneManager)
{
	mCamera = std::make_unique<Camera>(sceneManager,
	                                   mCameraData.mCameraPos,
	                                   mCameraData.mCameraFront,
	                                   mCameraData.mCameraUp);
}
