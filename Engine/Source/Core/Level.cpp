#include "FileSystem.h"

#include "Level.h"

Level::Level(const std::string& levelName)
{
	const auto json = FileSystem::ReadJson(levelName);
	_ParseJson(json);
}

void Level::_ParseJson(const nlohmann::json& json)
{
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

			mMapData = std::make_unique<MapData>();
			mMapData->name = mapName;
			mMapData->position = mapPosVec;
			mMapData->rotation = mapRotationVec;
			mMapData->tileSize = mapTileSize;
			mMapData->hasDimension = hasDimension;
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

			mEntityDefinitions.push_back({std::move(entityName), entityPosVec});
		}
	}

	if (json.contains(SCRIPTS_STRING))
	{
		for (auto& script : json[SCRIPTS_STRING])
		{
			mScripts.push_back(script);
		}
	}
}
