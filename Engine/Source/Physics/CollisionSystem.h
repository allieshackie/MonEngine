#pragma once

class EntityRegistry;
class EventPublisher;
class EventSubscription;
class MapSystem;

struct CollisionComponent;
struct TransformComponent;

class CollisionSystem
{
public:
	CollisionSystem(EventPublisher& eventPublisher);
	~CollisionSystem();

	void Update(EntityRegistry& entityRegistry) const;

private:
	void _CheckForCollisions(const CollisionComponent& collider, TransformComponent& transform);
	void _UpdatePositionIfColliding(TransformComponent& transform);

	bool _AABBCheck(const CollisionComponent& firstCollider, TransformComponent& firstTransform,
	                const CollisionComponent& secondCollider, TransformComponent& secondTransform);

	void _OnColliderAdded(int entityId, const std::type_info& typeInfo);
	void _OnColliderRemoved(int entityId, const std::type_info& typeInfo);

	std::vector<int> mEntitiesWithColliders;

	EventPublisher& mEventPublisher;

	std::shared_ptr<EventSubscription> mAddComponentSubscription;
	std::shared_ptr<EventSubscription> mRemoveComponentSubscription;
};
