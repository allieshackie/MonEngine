#include "Scene.h"
#include "Entity/Descriptions/DescriptionFactory.h"
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

	mCurrentWorld = std::make_shared<World>();
}

std::shared_ptr<World> SceneManager::GetCurrentWorld() const
{
	return mCurrentWorld;
}

void SceneManager::LoadScene(const std::string& SceneName, const MapRegistry& mapRegistry, LuaSystem& luaSystem) const
{
	if (mCurrentWorld != nullptr)
	{
		mCurrentWorld->Close();
	}

	// parse and serialize JSON
	std::string fullFileName = LEVELS_FOLDER;
	fullFileName.append(SceneName);

	auto scene = new MonScene();

	try
	{
		auto archive = FileSystem::CreateArchive(fullFileName, true);
		scene->load(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "MonScene deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	if (scene == nullptr)
	{
		printf("Scene Data could not be parsed");
		return;
	}

	mCurrentWorld->Init(scene, *mEntityTemplateRegistry, mapRegistry, luaSystem);
}

const std::vector<const char*>& SceneManager::GetSceneNames() const
{
	return mSceneFileNames;
}
