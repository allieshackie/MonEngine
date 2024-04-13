#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/EntityRegistry.h"

#include "CollisionSystem.h"

void CollisionSystem::Update(EntityRegistry& entityRegistry) const
{
	const auto staticView = entityRegistry.GetEnttRegistry().view<
		CollisionComponent, TransformComponent>(entt::exclude<PhysicsComponent>);
	const auto dynamicView = entityRegistry.GetEnttRegistry().view<
		CollisionComponent, TransformComponent, PhysicsComponent>();

	dynamicView.each([=](const auto& collider, const auto& transform, auto& physics)
	{
		staticView.each([=](const auto& staticCollider, const auto& staticTransform)
		{
		});
	});
}

void CollisionSystem::_CheckForCollisions(const CollisionComponent& collider, TransformComponent& transform,
                                          const CollisionComponent& secondCollision,
                                          TransformComponent& secondTransform)
{
	// AABB check
	if (collider.mColliderShape == ColliderShapes::Box)
	{
		// vs AABB check
		if (secondCollision.mColliderShape == ColliderShapes::Box)
		{
			_AABBCheck(collider, transform, secondCollision, secondTransform);
		}
	}
}

bool CollisionSystem::_AABBCheck(const CollisionComponent& firstCollider, TransformComponent& firstTransform,
                                 const CollisionComponent& secondCollider, TransformComponent& secondTransform)
{
	// Entity we're checking for
	const auto firstColliderWorldPosition = firstTransform.mPosition * firstCollider.mSize;

	const auto firstMin = firstColliderWorldPosition - (firstCollider.mSize / 2.0f);

	// Entity we're checking against
	const glm::vec3 secondColliderWorldPosition = secondTransform.mPosition * secondCollider.mSize;

	return false;
}
