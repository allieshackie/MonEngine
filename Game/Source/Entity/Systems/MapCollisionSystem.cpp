#include "Entity/EntityRegistry.h"

#include "MapCollisionSystem.h"

#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/TransformComponent.h"

MapCollisionSystem::MapCollisionSystem(CollisionSystem& collisionSystem, EntityRegistry& entityRegistry,
                                       MapSystem& mapSystem)
	: mCollisionSystem(collisionSystem), mEntityRegistry(entityRegistry), mMapSystem(mapSystem)
{
}

void MapCollisionSystem::Update()
{
	const auto view = mEntityRegistry.GetEnttRegistry().view<CollisionComponent, TransformComponent>();
	view.each([=](const auto& collider, auto& transform)
	{
	});
}
