#pragma once

class EntityRegistry;

struct CollisionComponent;
struct TransformComponent;

// TODO: optimize by only checking collision on shapes that have moved in the last frame

class CollisionSystem
{
public:
	void Update(EntityRegistry& entityRegistry) const;

private:
	void _CheckForCollisions(const CollisionComponent& collider, TransformComponent& transform,
	                         const CollisionComponent& secondCollision, TransformComponent& secondTransform);

	void _UpdatePositionIfColliding(TransformComponent& transform);

	bool _AABBCheck(const CollisionComponent& firstCollider, TransformComponent& firstTransform,
	                const CollisionComponent& secondCollider, TransformComponent& secondTransform);
};
