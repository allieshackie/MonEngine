#pragma once

class EntityRegistry;
class EventListener;
class MapSystem;

struct TransformComponent;

class CollisionSystem
{
public:
	CollisionSystem(EntityRegistry& entityRegistry, EventListener& eventListener);
	~CollisionSystem();

	void Update();

private:
	void _CheckForCollisions(const CollisionComponent& collider, TransformComponent& transform);
	void _UpdatePositionIfColliding(TransformComponent& transform);

	bool _AABBCheck(const CollisionComponent& firstCollider, TransformComponent& firstTransform,
	                const CollisionComponent& secondCollider, TransformComponent& secondTransform);

	void _OnColliderAdded(int entityId, const std::type_info& typeInfo);
	void _OnColliderRemoved(int entityId, const std::type_info& typeInfo);

	std::vector<int> mEntitiesWithColliders;

	EntityRegistry& mEntityRegistry;
	EventListener& mEventListener;
};
