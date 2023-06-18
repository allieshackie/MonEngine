#include "Components/CollisionComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/TransformComponent.h"
#include "EntityRegistry.h"
#include "EventListener.h"

#include "CollisionSystem.h"

#include "Math/Vector.h"

// TODO: Potential collision detection methods to explore: AABB, OBB, Sweep and Prune, Hierarchical Grids, Spatial Partitioning

CollisionSystem::CollisionSystem(EntityRegistry& entityRegistry, EventListener& eventListener)
	: mEntityRegistry(entityRegistry), mEventListener(eventListener)
{
	mEventListener.AddListener("component_added", [this](int entityId, const std::type_info& typeInfo)
	{
		_OnColliderAdded(entityId, typeInfo);
	});
	mEventListener.AddListener("component_removed", [this](int entityId, const std::type_info& typeInfo)
	{
		_OnColliderRemoved(entityId, typeInfo);
	});
}

CollisionSystem::~CollisionSystem()
{
	mEventListener.RemoveListener("component_added", [this](int entityId, const std::type_info& typeInfo)
	{
		_OnColliderAdded(entityId, typeInfo);
	});
	mEventListener.RemoveListener("component_removed", [this](int entityId, const std::type_info& typeInfo)
	{
		_OnColliderRemoved(entityId, typeInfo);
	});
}

void CollisionSystem::Update()
{
	const auto view = mEntityRegistry.GetEnttRegistry().view<
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
	const MonDev::Vector3 firstColliderWorldPosition = firstTransform.mPosition * firstCollider.mSize;

	const glm::vec3 firstMin = firstColliderWorldPosition - (firstCollider.mSize / 2);

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
