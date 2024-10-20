#pragma once
#include <glm/vec3.hpp>

struct MapData
{
	std::string name;
	glm::vec3 position = {0, 0, 0};
	glm::vec3 size = {0, 0, 0};
	glm::vec3 rotation = {0, 0, 0};
	glm::vec4 color = {0, 0, 0, 0};
	float tileSize = 0.0f;
	bool hasDimension = false;
};

struct EntityData
{
	std::string name;
	glm::vec3 position;
};

struct Level
{
	MapData mMapData;
	std::vector<EntityData> mEntityDefinitions;
	std::vector<std::string> mScripts;
};
