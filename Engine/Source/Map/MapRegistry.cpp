#include "Core/EngineContext.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/TransformComponent.h"

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
	fullMapPath.append(mapData.mName);

	std::ifstream jsonStream(fullMapPath);
	if (!jsonStream.is_open())
	{
		std::cerr << "Error: Could not open file " << fullMapPath << std::endl;
		return false;
	}

	TempMapReg mapReg;
	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		mapReg.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "TempMapReg deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	std::string fullMapDataPath = MAPS_FOLDER;
	fullMapDataPath.append(mapReg.mDataPath);
	std::ifstream file(fullMapDataPath.c_str());

	std::string str;
	std::vector<int> data;

	while (std::getline(file, str, ','))
	{
		data.emplace_back(std::stoi(str));
	}

	entityReg.AddComponentWithArgs<MapComponent>(entityId, mapData.mName, mapReg.mRows, mapReg.mColumns,
	                                             mapReg.mTextureSize, mapReg.mTextureData.mTexturePath,
	                                             mapReg.mTextureData.mTextureRows, mapReg.mTextureData.mTextureColumns,
	                                             mapReg.mDataPath, data);

	entityReg.AddComponentWithArgs<TransformComponent>(entityId, mapData.mPosition, mapData.mSize, mapData.mRotation);

	if (mapData.mHasDimension)
	{
		entityReg.AddComponentWithArgs<MeshComponent>(entityId, "PlainBox.gltf");
	}
	else
	{
		entityReg.AddComponentWithArgs<MeshComponent>(entityId, "Plane.gltf");
	}

	entityReg.AddComponentWithArgs<CollisionComponent>(entityId, ColliderShapes::BOX, mapData.mSize, false, -1, nullptr,
	                                                   true);

	return mapReg.mTextureData.mTextureRows != 0 && mapReg.mTextureData.mTextureColumns != 0;
}
