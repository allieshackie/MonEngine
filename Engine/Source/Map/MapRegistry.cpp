#include "Core/EngineContext.h"
#include "Core/FileSystem.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "MapRegistry.h"

void MapRegistry::OpenMap(const EngineContext& context, const MapData& mapData) const
{
	// Create entity
	auto& entityReg = context.GetEntityRegistry();
	const auto entityId = entityReg.CreateEntity();

	_ParseMapData(mapData, entityId, entityReg);

	// Generate texture
	context.GenerateMapTexture(entityId);
}

void MapRegistry::CloseMap(const std::string& mapId)
{
}

void MapRegistry::_ParseMapData(const MapData& mapData, EntityId entityId, EntityRegistry& entityReg) const
{
	std::string fullMapPath = MAPS_FOLDER;
	fullMapPath.append(mapData.name);

	const auto json = FileSystem::ReadJson(fullMapPath);
	assert(json.contains(ID_STRING));
	std::string id = json[ID_STRING];

	assert(json.contains(ROWS_STRING));
	int rows = json[ROWS_STRING];
	assert(json.contains(COLUMNS_STRING));
	int columns = json[COLUMNS_STRING];

	assert(json.contains(TEXTURE_DATA_STRING));
	const auto textureData = json[TEXTURE_DATA_STRING];
	std::string texturePath = textureData[TEXTURE_PATH_STRING];
	int textureRows = textureData[ROWS_STRING];
	int textureColumns = textureData[COLUMNS_STRING];

	assert(json.contains(TEXTURE_SIZE_STRING));
	auto textureSize = json[TEXTURE_SIZE_STRING];
	std::array<uint32_t, 2> textureDimension = {textureSize[0], textureSize[1]};

	assert(json.contains(DATA_PATH_STRING));
	std::string dataPath = json[DATA_PATH_STRING];

	std::string fullMapDataPath = MAPS_FOLDER;
	fullMapDataPath.append(dataPath);
	std::ifstream file(fullMapDataPath.c_str());

	std::string str;
	std::vector<int> data;

	while (std::getline(file, str, ','))
	{
		data.emplace_back(std::stoi(str));
	}

	entityReg.AddComponent<MapComponent>(entityId, mapData.name, 0, rows, columns, textureDimension, texturePath,
	                                     textureRows, textureColumns, dataPath, data);

	auto size = glm::vec3{
		static_cast<float>(columns) * mapData.tileSize,
		static_cast<float>(rows) * mapData.tileSize, 1
	};
	entityReg.AddComponent<TransformComponent>(entityId, mapData.position, size, mapData.rotation);
}
