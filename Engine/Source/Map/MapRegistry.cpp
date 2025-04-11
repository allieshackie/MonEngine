#include "Core/Scene.h"
#include "Entity/Entity.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Util/FileSystem.h"

#include "MapRegistry.h"

void MapRegistry::OpenMap(MonScene* scene, const MapData& mapData) const
{
	// Create entity
	auto& entity = scene->CreateEntity();

	if (_ParseMapData(mapData, entity))
	{
		// TODO: Generate texture
		//context.GenerateMapTexture(entityId);
	}
}

void MapRegistry::CloseMap(const std::string& mapId)
{
}

bool MapRegistry::_ParseMapData(const MapData& mapData, Entity& entity) const
{
	std::string fullMapPath = MAPS_FOLDER;
	fullMapPath.append(mapData.mName);

	MapComponent mapComponent;
	try
	{
		auto archive = FileSystem::CreateArchive(fullMapPath, true);
		mapComponent.load(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "TempMapReg deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	std::string fullMapDataPath = MAPS_FOLDER;
	fullMapDataPath.append(mapComponent.mDataPath);
	std::ifstream file(fullMapDataPath.c_str());

	std::string str;
	while (std::getline(file, str, ','))
	{
		mapComponent.mData.emplace_back(std::stoi(str));
	}

	entity.SetName(mapData.mName);
	entity.AddComponent<MapComponent>(mapComponent);
	entity.AddComponentWithArgs<TransformComponent>(mapData.mPosition, mapData.mSize, mapData.mRotation);
	entity.AddComponentWithArgs<MeshComponent>(mapComponent.mMeshPath);
	entity.AddComponentWithArgs<CollisionComponent>(ColliderShapes::BOX, mapData.mSize,
	                                                false, -1, nullptr, true);

	return mapComponent.mTextureTiling.x != 0.0f && mapComponent.mTextureTiling.y != 0.0f;
}
