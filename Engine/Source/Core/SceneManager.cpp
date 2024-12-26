#include "Core/EngineContext.h"
#include "Entity/EntityTemplateRegistry.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Map/MapRegistry.h"
#include "Util/FileSystem.h"

#include "SceneManager.h"

SceneManager::SceneManager(DescriptionFactory& descriptionFactory)
{
	mEntityTemplateRegistry = std::make_unique<EntityTemplateRegistry>(descriptionFactory);

	mSceneFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(LEVELS_FOLDER))
	{
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_SCENE) != std::string::npos) continue;
		mSceneFileNames.push_back(_strdup(fileName.c_str()));
	}
}

const MonScene* SceneManager::GetCurrentScene() const
{
	return mCurrentScene.get();
}

void SceneManager::LoadScene(const std::string& SceneName, const EngineContext& context, MapRegistry& mapRegistry,
                             LuaSystem& luaSystem)
{
	if (mCurrentScene != nullptr)
	{
		_UnloadScene(context, mapRegistry, luaSystem);
	}

	// parse and serialize JSON
	std::string fullFileName = LEVELS_FOLDER;
	fullFileName.append(SceneName);

	_ParseSceneJson(fullFileName, context);

	if (mCurrentScene == nullptr)
	{
		printf("Scene Data could not be parsed");
		return;
	}

	// Create Map
	if (!mCurrentScene->mMapData.mName.empty())
	{
		mapRegistry.OpenMap(context, mCurrentScene->mMapData);
	}

	for (const auto& entity : mCurrentScene->mEntityDefinitions)
	{
		const auto gameObj = CreateEntityFromTemplate(entity.mName.c_str());
		auto& transformComponent = gameObj->GetComponent<TransformComponent>();
		transformComponent.mPosition = entity.mPosition;
		if (const auto collider = gameObj->TryGetComponent<CollisionComponent>(); collider != nullptr)
		{
			collider->mInitialized = true;
		}
	}

	for (const auto& script : mCurrentScene->mScripts)
	{
		luaSystem.LoadScript(script.c_str(), context);
	}
}

Entity* SceneManager::CreateEntityFromTemplate(const char* templateName)
{
	const auto& descriptions = mEntityTemplateRegistry->GetEntityTemplateDescriptions(templateName);
	const auto entity = CreateEntity();

	for (const auto& description : descriptions)
	{
		description->ApplyToEntity(entity, *mCurrentScene->mRegistry);
	}

	return entity;
}

Entity* SceneManager::CreateEntity() const
{
	auto id = mCurrentScene->mRegistry.create();
	return new Entity(id);
}

void SceneManager::RemoveEntity(const entt::entity id) const
{
	mCurrentScene->mRegistry.destroy(id);
}

const std::vector<const char*>& SceneManager::GetSceneNames() const
{
	return mSceneFileNames;
}

void SceneManager::_UnloadScene(const EngineContext& context, MapRegistry& mapRegistry,
                                LuaSystem& luaSystem) const
{
	luaSystem.QueueClose();

	if (!mCurrentScene->mMapData.mName.empty())
	{
		mapRegistry.CloseMap(mCurrentScene->mMapData.mName);
	}

	context.FlushEntities();
}

void SceneManager::_ParseSceneJson(const std::string& sceneName, const EngineContext& context)
{
	mCurrentScene = std::make_shared<MonScene>();
	std::ifstream jsonStream(sceneName.c_str());

	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		mCurrentScene->load(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "MonScene deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	mCurrentScene->mCamera = std::make_unique<Camera>(context.GetEntityRegistry(),
	                                                  mCurrentScene->mCameraData.mCameraPos,
	                                                  mCurrentScene->mCameraData.mCameraFront,
	                                                  mCurrentScene->mCameraData.mCameraUp);
}
