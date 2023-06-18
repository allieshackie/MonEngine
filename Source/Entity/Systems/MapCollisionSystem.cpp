#include "MapCollisionSystem.h"

MapCollisionSystem::MapCollisionSystem(CollisionSystem& collisionSystem, MapSystem& mapSystem)
	: mCollisionSystem(collisionSystem), mMapSystem(mapSystem)
{
}

void MapCollisionSystem::Update()
{
}
