#include "Core/Scene.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "MapCollisionSystem.h"

MapCollisionSystem::MapCollisionSystem(CollisionSystem& collisionSystem,
                                       MapSystem& mapSystem)
	: mCollisionSystem(collisionSystem), mMapSystem(mapSystem)
{
}

void MapCollisionSystem::Update(MonScene* scene)
{
	if (scene == nullptr) return;
	const auto view = scene->GetRegistry().view<CollisionComponent, TransformComponent>();
	view.each([=](const auto& collider, auto& transform)
	{
	});
}
