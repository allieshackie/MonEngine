#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/EntityRegistry.h"
#include "Core/EventListener.h"

#include "CollisionSystem.h"

// TODO: Potential collision detection methods to explore: AABB, OBB, Sweep and Prune, Hierarchical Grids, Spatial Partitioning

CollisionSystem::CollisionSystem(EventPublisher& eventPublisher)
	: mEventPublisher(eventPublisher)
{
	EventFunc addedFunc = [this](int entityId, const std::type_info& typeInfo)
	{
		_OnColliderAdded(entityId, typeInfo);
	};

	mAddComponentSubscription = mEventPublisher.AddListener("component_added", addedFunc);

	EventFunc removedFunc = [this](int entityId, const std::type_info& typeInfo)
	{
		_OnColliderAdded(entityId, typeInfo);
	};

	mRemoveComponentSubscription = mEventPublisher.AddListener("component_removed", removedFunc);
}

CollisionSystem::~CollisionSystem()
{
	mEventPublisher.RemoveListener(mAddComponentSubscription);
	mEventPublisher.RemoveListener(mRemoveComponentSubscription);
}

void CollisionSystem::Update(EntityRegistry& entityRegistry) const
{
	const auto view = entityRegistry.GetEnttRegistry().view<
		CollisionComponent, TransformComponent>();
	view.each([=](const auto& collider, auto& transform)
	{
	});
}

void CollisionSystem::_CheckForCollisions(const CollisionComponent& collider, TransformComponent& transform)
{
	// AABB check
	if (collider.mColliderShape == ColliderShapes::Box)
	{
	}
}

void CollisionSystem::_UpdatePositionIfColliding(TransformComponent& transform)
{
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

void CollisionSystem::_OnColliderAdded(int entityId, const std::type_info& typeInfo)
{
	if (typeInfo == typeid(CollisionComponent))
	{
		mEntitiesWithColliders.push_back(entityId);
	}
}

void CollisionSystem::_OnColliderRemoved(int entityId, const std::type_info& typeInfo)
{
	if (typeInfo == typeid(CollisionComponent))
	{
		mEntitiesWithColliders.erase(
			std::remove(mEntitiesWithColliders.begin(), mEntitiesWithColliders.end(), entityId),
			mEntitiesWithColliders.end());
	}
}
