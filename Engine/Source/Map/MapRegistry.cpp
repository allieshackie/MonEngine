#include "Core/EngineContext.h"
#include "Core/Scene.h"
#include "Entity/Entity.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/TransformComponent.h"

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

	TempMapReg mapReg;
	try
	{
		auto archive = FileSystem::CreateArchive(fullMapPath, true);
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

	entity.AddComponentWithArgs<MapComponent>(mapData.mName, mapReg.mRows, mapReg.mColumns,
	                                          mapReg.mTextureSize, mapReg.mTextureData.mTexturePath,
	                                          mapReg.mTextureData.mTextureRows, mapReg.mTextureData.mTextureColumns,
	                                          mapReg.mDataPath, data);

	entity.AddComponentWithArgs<TransformComponent>(mapData.mPosition, mapData.mSize, mapData.mRotation);

	if (mapData.mHasDimension)
	{
		entity.AddComponentWithArgs<MeshComponent>("PlainBox.gltf");
	}
	else
	{
		entity.AddComponentWithArgs<MeshComponent>("Plane.gltf");
	}

	entity.AddComponentWithArgs<CollisionComponent>(ColliderShapes::BOX, mapData.mSize, false, -1, nullptr,
	                                                true);

	return mapReg.mTextureData.mTextureRows != 0 && mapReg.mTextureData.mTextureColumns != 0;
}
