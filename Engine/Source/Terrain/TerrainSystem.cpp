#include "Core/Scene.h"
#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Util/FileSystem.h"

#include "TerrainSystem.h"

void TerrainSystem::CreateTerrain(World* world, const TerrainData& data) const
{
	auto& entity = world->CreateEntity();
	_ParseData(data, entity);
}

void TerrainSystem::DestroyTerrain(const std::string& mapId)
{
}

void TerrainSystem::_ParseData(const TerrainData& data, Entity& entity) const
{
	std::string fullMapPath = ASSETS_FOLDER;
	fullMapPath.append(data.mName);

	entity.SetName(data.mName);
	entity.AddComponentWithArgs<TransformComponent>(data.mPosition, data.mSize, data.mRotation);
	entity.AddComponentWithArgs<ModelComponent>(fullMapPath);
	entity.AddComponentWithArgs<CollisionComponent>(ColliderShapes::BOX, data.mSize, false, -1, nullptr, true);
}
