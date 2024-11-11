#include "Core/EngineContext.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Util/FileSystem.h"

#include "MapRegistry.h"

void MapRegistry::OpenMap(const EngineContext& context, const MapData& mapData) const
{
	// Create entity
	auto& entityReg = context.GetEntityRegistry();
	const auto entityId = entityReg.CreateEntity();

	if (_ParseMapData(mapData, entityId, entityReg))
	{
		// Generate texture
		context.GenerateMapTexture(entityId);
	}
}

void MapRegistry::CloseMap(const std::string& mapId)
{
}

bool MapRegistry::_ParseMapData(const MapData& mapData, EntityId entityId, EntityRegistry& entityReg) const
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

	int textureRows = 0;
	if (textureData.contains(ROWS_STRING))
	{
		textureRows = textureData[ROWS_STRING];
	}

	int textureColumns = 0;
	if (textureData.contains(COLUMNS_STRING))
	{
		textureColumns = textureData[COLUMNS_STRING];
	}

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

	entityReg.AddComponentWithArgs<MapComponent>(entityId, mapData.name, rows, columns, textureDimension,
	                                             texturePath, textureRows, textureColumns, dataPath, data);

	entityReg.AddComponentWithArgs<TransformComponent>(entityId, mapData.position, mapData.size, mapData.rotation);

	if (mapData.hasDimension)
	{
		entityReg.AddComponentWithArgs<MeshComponent>(entityId, "PlainBox.gltf");
	}
	else
	{
		entityReg.AddComponentWithArgs<MeshComponent>(entityId, "Plane.gltf");
	}

	entityReg.AddComponentWithArgs<CollisionComponent>(entityId, ColliderShapes::BOX, mapData.size, false, -1, nullptr,
	                                                   true);

	return textureRows != 0 && textureColumns != 0;
}
