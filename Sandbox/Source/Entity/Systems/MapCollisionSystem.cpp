#include "Core/World.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "MapCollisionSystem.h"

MapCollisionSystem::MapCollisionSystem(CollisionSystem& collisionSystem, MapSystem& mapSystem,
                                       std::weak_ptr<World> world)
	: mCollisionSystem(collisionSystem), mMapSystem(mapSystem), mWorld(std::move(world))
{
}

void MapCollisionSystem::Update(float dt)
{
	if (const auto world = mWorld.lock())
	{
		const auto view = world->GetRegistry().view<CollisionComponent, TransformComponent>();
		view.each([=](const auto& collider, auto& transform)
		{
		});
	}
}
