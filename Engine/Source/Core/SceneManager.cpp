#include "Scene.h"
#include "Graphics/RenderSystem.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Util/FileSystem.h"
#include "Util/LuaUtil.h"

#include "SceneManager.h"

SceneManager::SceneManager(DescriptionFactory& descriptionFactory, RenderSystem& renderSystem, ResourceManager& resourceManager, EventPublisher& eventPublisher)
	: LuaBindable("SceneManager"), mRenderSystem(renderSystem), mResourceManager(resourceManager), mEventPublisher(eventPublisher)
{
	mPrefabRegistry = std::make_unique<PrefabRegistry>(descriptionFactory);

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

	//mCurrentWorld = std::make_shared<World>();
}

std::shared_ptr<World> SceneManager::GetCurrentWorld() const
{
	return mCurrentWorld;
}

void SceneManager::LoadScene(const std::string& sceneName)
{
	if (mCurrentWorld != nullptr)
	{
		mCurrentWorld->Close();
	}

	mCurrentSceneName = sceneName;

	// parse and serialize JSON
	std::string fullFileName = LEVELS_FOLDER;
	fullFileName.append(sceneName);

	MonScene scene;

	try
	{
		auto archive = FileSystem::CreateArchive(fullFileName, true);
		scene.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "MonScene deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	mCurrentWorld = std::make_shared<World>();
	mEventPublisher.NotifyWorldCreated(mCurrentWorld);

	mCurrentWorld->Init(scene, *mPrefabRegistry, mRenderSystem, mResourceManager, mLuaSystem);
}

void SceneManager::RestartScene()
{
	if (!mCurrentSceneName.empty())
	{
		LoadScene(mCurrentSceneName);
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
