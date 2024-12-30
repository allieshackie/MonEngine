#include "EventListener.h"
#include "Core/EngineContext.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Map/MapRegistry.h"
#include "Util/FileSystem.h"

#include "SceneManager.h"

SceneManager::SceneManager(DescriptionFactory& descriptionFactory)
{
	mEventPublisher = std::make_unique<EventPublisher>();
	mEntityTemplateRegistry = std::make_unique<EntityTemplateRegistry>(descriptionFactory);

	mSceneFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(LEVELS_FOLDER))
	{
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_SCENE) != std::string::npos) continue;
		mSceneFileNames.push_back(_strdup(fileName.c_str()));
	}
}

MonScene* SceneManager::GetCurrentScene() const
{
	if (mCurrentScene)
	{
		return mCurrentScene.get();
	}
	return nullptr;
}

void SceneManager::LoadScene(const std::string& SceneName, const EngineContext& context, MapRegistry& mapRegistry,
                             LuaSystem& luaSystem)
{
	if (mCurrentScene != nullptr)
	{
		_UnloadScene(mapRegistry, luaSystem);
	}

	// parse and serialize JSON
	std::string fullFileName = LEVELS_FOLDER;
	fullFileName.append(SceneName);

	_ParseSceneJson(fullFileName);

	if (mCurrentScene == nullptr)
	{
		printf("Scene Data could not be parsed");
		return;
	}

	// Create Map
	if (!mCurrentScene->GetMapData().mName.empty())
	{
		mapRegistry.OpenMap(mCurrentScene.get(), mCurrentScene->GetMapData());
	}

	for (const auto& entity : mCurrentScene->GetEntityDefinitions())
	{
		auto gameObj = CreateEntityFromTemplate(entity.mName.c_str());
		auto& transformComponent = gameObj.GetComponent<TransformComponent>();
		transformComponent.mPosition = entity.mPosition;
		if (const auto collider = gameObj.TryGetComponent<CollisionComponent>(); collider != nullptr)
		{
			collider->mInitialized = true;
		}
	}

	for (const auto& script : mCurrentScene->GetScripts())
	{
		luaSystem.LoadScript(script.c_str(), context);
	}
}

Entity& SceneManager::CreateEntityFromTemplate(const char* templateName) const
{
	return mCurrentScene->CreateEntityFromTemplate(templateName, *mEntityTemplateRegistry);
}

Entity& SceneManager::CreateEntity() const
{
	return mCurrentScene->CreateEntity();
}

void SceneManager::RemoveEntity(const entt::entity id) const
{
	mCurrentScene->RemoveEntity(id);
}

void SceneManager::FlushEntities() const
{
	mCurrentScene->FlushEntities();
}

const std::vector<const char*>& SceneManager::GetSceneNames() const
{
	return mSceneFileNames;
}

void SceneManager::_UnloadScene(MapRegistry& mapRegistry,
                                LuaSystem& luaSystem) const
{
	luaSystem.QueueClose();

	if (!mCurrentScene->GetMapData().mName.empty())
	{
		mapRegistry.CloseMap(mCurrentScene->GetMapData().mName);
	}

	FlushEntities();
}

void SceneManager::_ParseSceneJson(const std::string& sceneName)
{
	mCurrentScene = std::make_unique<MonScene>(*mEventPublisher);

	try
	{
		auto archive = FileSystem::CreateArchive(sceneName, true);
		mCurrentScene->load(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "MonScene deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	mCurrentScene->CreateCamera(*this);
}
