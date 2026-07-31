#include "Scene.h"
#include "Graphics/RenderSystem.h"
#include "Entity/PrefabRegistry.h"
#include "Util/FileSystem.h"
#include "Util/LuaUtil.h"

#include "SceneManager.h"

SceneManager::SceneManager(PrefabRegistry& prefabRegistry, RenderSystem& renderSystem, ResourceManager& resourceManager, EventPublisher& eventPublisher)
	: LuaBindable("SceneManager"), mRenderSystem(renderSystem), mResourceManager(resourceManager), mEventPublisher(eventPublisher), mPrefabRegistry(prefabRegistry)
{
	mSceneFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(LEVELS_FOLDER))
	{
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_SCENE) != std::string::npos)
		{
			continue;
		}
		mSceneFileNames.push_back(_strdup(fileName.c_str()));
	}
}

std::shared_ptr<World> SceneManager::GetCurrentWorld() const
{
	return mCurrentWorld;
}

void SceneManager::LoadScene(const std::string& sceneName)
{
	// Cleanup previous scene
	CloseScene();

	mCurrentSceneName = sceneName;

	// parse and serialize JSON
	std::string fullFileName = LEVELS_FOLDER;
	fullFileName.append(sceneName);

	MonScene scene;

	try
	{
		auto archive = FileSystem::CreateArchive(fullFileName, true);
		scene.serialize(archive);
		auto readJson = FileSystem::ReadJson(fullFileName);
		if (readJson.find("entities") != readJson.end())
		{
			for (const auto& [entityName, entityJson] : readJson["entities"].items())
			{
				EntityData data;
				data.mPrefab = entityJson["prefab"];
				for (const auto& [componentName, componentJson] : entityJson["components"].items())
				{
					const auto& componentInfo = mPrefabRegistry.GetComponentLoader(componentName);
					data.mOverrides.push_back({ componentName, componentInfo, componentJson });
				}
				scene.AddEntityOverride(data);
			}
		}
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "MonScene deserialization error: " << e.what() << std::endl;
		assert(false);
	}


	mCurrentWorld = std::make_shared<World>();
	mEventPublisher.Notify(mCurrentWorld);

	mCurrentWorld->Init(scene, mPrefabRegistry, mRenderSystem, mResourceManager, mLuaSystem);
}

void SceneManager::RestartScene()
{
	if (!mCurrentSceneName.empty())
	{
		LoadScene(mCurrentSceneName);
	}
}

void SceneManager::CloseScene()
{
	if (mCurrentWorld != nullptr)
	{
		mCurrentWorld->Close();
		mCurrentWorld = nullptr;
	}
}

const std::vector<const char*>& SceneManager::GetSceneNames() const
{
	return mSceneFileNames;
}

void SceneManager::BindMethods(lua_State* state)
{
	LuaUtil::RegisterMethod<SceneManager, &SceneManager::LoadScene>(state, "LoadScene");
}

void SceneManager::BindInstanceGetter(lua_State* state)
{
	LuaUtil::RegisterInstanceGetter(state, "GetSceneManager", this);
}
