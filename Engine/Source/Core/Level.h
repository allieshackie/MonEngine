#pragma once
#include <nlohmann/json.hpp>
#include <glm/vec3.hpp>

#include "Camera.h"

struct MapData
{
	std::string name;
	glm::vec3 position;
	glm::vec3 rotation;
	float tileSize = 0.0f;
};

struct EntityData
{
	std::string name;
	glm::vec3 position;
};

// TODO: Level definition, we'll need to save out level later
class Level
{
public:
	Level(const std::string& levelName);

	const std::unique_ptr<Camera>& GetCamera() const { return mCamera; }

	const std::unique_ptr<MapData>& GetMapData() const { return mMapData; }
	const std::vector<EntityData>& GetEntityDefinitions() const { return mEntityDefinitions; }

private:
	void _ParseJson(const nlohmann::json& json);

	static constexpr char CAMERA_STRING[] = "camera";
	static constexpr char POSITION_STRING[] = "position";
	static constexpr char FRONT_STRING[] = "front";
	static constexpr char UP_STRING[] = "up";

	static constexpr char MAPS_STRING[] = "maps";
	static constexpr char NAME_STRING[] = "name";
	static constexpr char ROTATION_STRING[] = "rotation";
	static constexpr char TILE_SIZE_STRING[] = "tile_size";

	static constexpr char ENTITIES_STRING[] = "entities";

	// runtime data
	std::unique_ptr<Camera> mCamera = nullptr;

	// initialization data
	std::unique_ptr<MapData> mMapData = nullptr;
	std::vector<EntityData> mEntityDefinitions;
};
