#include "FileSystem.h"
#include "Core/EngineContext.h"
#include "Entity/Components/TransformComponent.h"
#include "Map/MapRegistry.h"

#include "LevelManager.h"

LevelManager::LevelManager()
{
	mLevelFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(LEVELS_FOLDER))
	{
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_LEVEL) != std::string::npos) continue;
		mLevelFileNames.push_back(_strdup(fileName.c_str()));
	}
}

const Level* LevelManager::GetCurrentLevel() const
{
	return mCurrentLevel.get();
}

void LevelManager::LoadLevel(const std::string& levelName, const EngineContext& context, MapRegistry& mapRegistry,
                             LuaSystem& luaSystem)
{
	if (mCurrentLevel != nullptr)
	{
		_UnloadLevel(context, mapRegistry, luaSystem);
	}

	// parse and serialize JSON
	std::string fullFileName = LEVELS_FOLDER;
	fullFileName.append(levelName);

	_ParseLevelJson(fullFileName);

	if (mCurrentLevel == nullptr)
	{
		printf("Level Data could not be parsed");
		return;
	}

	// Create Map
	if (!mCurrentLevel->mMapData.name.empty())
	{
		mapRegistry.OpenMap(context, mCurrentLevel->mMapData);
	}

	for (const auto& entity : mCurrentLevel->mEntityDefinitions)
	{
		const auto entityId = context.CreateGameObject(entity.name);
		auto& transformComponent = context.GetComponent<TransformComponent>(entityId);
		transformComponent.mPosition = entity.position;
	}

	for (const auto& script : mCurrentLevel->mScripts)
	{
		luaSystem.LoadScript(script.c_str(), context);
	}
}

const std::vector<const char*>& LevelManager::GetLevelNames() const
{
	return mLevelFileNames;
}

void LevelManager::_UnloadLevel(const EngineContext& context, MapRegistry& mapRegistry,
                                LuaSystem& luaSystem) const
{
	luaSystem.QueueClose();

	if (!mCurrentLevel->mMapData.name.empty())
	{
		mapRegistry.CloseMap(mCurrentLevel->mMapData.name);
	}

	context.FlushEntities();
}

void LevelManager::_ParseLevelJson(const std::string& levelName)
{
	mCurrentLevel = std::make_unique<Level>();
	const auto json = FileSystem::ReadJson(levelName);

	assert(json.contains(CAMERA_STRING));
	const auto& cameraJson = json[CAMERA_STRING];
	const auto& position = cameraJson[POSITION_STRING];
	assert(position.size() == 3);
	auto positionVec = glm::vec3(position[0], position[1], position[2]);

	const auto& front = cameraJson[FRONT_STRING];
	assert(front.size() == 3);
	auto frontVec = glm::vec3(front[0], front[1], front[2]);

	const auto& up = cameraJson[UP_STRING];
	assert(up.size() == 3);
	auto upVec = glm::vec3(up[0], up[1], up[2]);

	mCamera = std::make_unique<Camera>(positionVec, frontVec, upVec);

	if (json.contains(MAPS_STRING))
	{
		for (auto& mapData : json[MAPS_STRING])
		{
			const auto& mapName = mapData[NAME_STRING];

			const auto& mapPos = mapData[POSITION_STRING];
			assert(mapPos.size() == 3);
			auto mapPosVec = glm::vec3(mapPos[0], mapPos[1], mapPos[2]);

			const auto& mapRotation = mapData[ROTATION_STRING];
			assert(mapRotation.size() == 3);
			auto mapRotationVec = glm::vec3(mapRotation[0], mapRotation[1], mapRotation[2]);

			float mapTileSize = mapData[TILE_SIZE_STRING];

			bool hasDimension = false;
			if (mapData.contains(HAS_DIMENSION_STRING))
			{
				hasDimension = mapData[HAS_DIMENSION_STRING];
			}

			mCurrentLevel->mMapData.name = mapName;
			mCurrentLevel->mMapData.position = mapPosVec;
			mCurrentLevel->mMapData.rotation = mapRotationVec;
			mCurrentLevel->mMapData.tileSize = mapTileSize;
			mCurrentLevel->mMapData.hasDimension = hasDimension;
		}
	}

	if (json.contains(ENTITIES_STRING))
	{
		for (auto& entityData : json[ENTITIES_STRING])
		{
			// Keep entity definitions for later so they can be parsed and instantiated
			assert(entityData.contains(NAME_STRING));
			std::string entityName = entityData[NAME_STRING];

			assert(entityData.contains(POSITION_STRING));
			const auto& entityPos = entityData[POSITION_STRING];
			assert(entityPos.size() == 3);
			auto entityPosVec = glm::vec3(entityPos[0], entityPos[1], entityPos[2]);

			mCurrentLevel->mEntityDefinitions.push_back({std::move(entityName), entityPosVec});
		}
	}

	if (json.contains(SCRIPTS_STRING))
	{
		for (auto& script : json[SCRIPTS_STRING])
		{
			mCurrentLevel->mScripts.push_back(script);
		}
	}

	/*
	 *
	if (json.contains(LIGHTS_STRING))
	{
		for (auto& light : json[LIGHTS_STRING])
		{
			assert(light.contains(POSITION_STRING));
			const auto& pos = light[POSITION_STRING];
			assert(pos.size() == 3);
			auto posVec = glm::vec3(pos[0], pos[1], pos[2]);

			assert(light.contains(COLOR_STRING));
			const auto& color = light[COLOR_STRING];
			assert(color.size() == 3);
			auto colorVec = glm::vec3(color[0], color[1], color[2]);

			assert(light.contains(INTENSITY_STRING));

			mCurrentLevel->mLights.push_back(Light{posVec, colorVec, light[INTENSITY_STRING]});
		}
	}
	 */
}
