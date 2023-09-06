#include "Defines.h"

#include "Level.h"

Level::Level(const std::string& levelName)
{
	std::ifstream ifs(levelName.c_str());

	const auto json = nlohmann::json::parse(ifs, nullptr, false, true);
	_ParseJson(json);
}

void Level::_ParseJson(const nlohmann::json& json)
{
	assert(json.contains(CAMERA_STRING));
	const auto cameraJson = json[CAMERA_STRING];
	const auto position = cameraJson[POSITION_STRING];
	assert(position.size() == 3);
	auto positionVec = glm::vec3(position[0], position[1], position[2]);

	const auto front = cameraJson[FRONT_STRING];
	assert(front.size() == 3);
	auto frontVec = glm::vec3(front[0], front[1], front[2]);

	const auto up = cameraJson[UP_STRING];
	assert(up.size() == 3);
	auto upVec = glm::vec3(up[0], up[1], up[2]);

	mCamera = std::make_unique<Camera>(positionVec, frontVec, upVec);

	if (json.contains(MAPS_STRING))
	{
		for (auto& mapData : json[MAPS_STRING])
		{
			const auto mapName = mapData[NAME_STRING];

			const auto mapPos = mapData[POSITION_STRING];
			assert(mapPos.size() == 3);
			auto mapPosVec = glm::vec3(mapPos[0], mapPos[1], mapPos[2]);

			const auto mapRotation = mapData[ROTATION_STRING];
			assert(mapRotation.size() == 3);
			auto mapRotationVec = glm::vec3(mapRotation[0], mapRotation[1], mapRotation[2]);

			float mapTileSize = mapData[TILE_SIZE_STRING];
			mMapData = std::make_unique<MapData>(mapName, mapPosVec, mapRotationVec, mapTileSize);
		}
	}

	if (json.contains(ENTITIES_STRING))
	{
		for (auto& entityData : json[ENTITIES_STRING])
		{
			// Keep entity definitions for later so they can be parsed and instantiated
			mEntityDefinitions.push_back(entityData);
		}
	}
}
