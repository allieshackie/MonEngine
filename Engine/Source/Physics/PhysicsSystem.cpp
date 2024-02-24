#include <glm/geometric.hpp>

#include "Entity/EntityRegistry.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "PhysicsSystem.h"

static float DEFAULT_GRAVITY = 9.8f;
static float DEFAULT_FRICTION = 0.2f;
static float MIN_VELOCITY = 0.1f;

static void _ApplyGravity(PhysicsComponent& physics)
{
	if (physics.mIsDynamic)
	{
		physics.mAcceleration.y -= DEFAULT_GRAVITY;
	}
}

static void _TickMovement(float deltaTime, PhysicsComponent& physics, TransformComponent& transform)
{
	// integrate velocity
	const auto& pos = transform.mPosition;
	const auto& velocity = physics.mVelocity;
	float friction = physics.mFriction;

	const glm::vec3 newVelocity = velocity * friction;
	physics.mVelocity = newVelocity;

	transform.mPosition = {
		pos.x + newVelocity.x * deltaTime, pos.y + newVelocity.y * deltaTime, pos.z + newVelocity.z * deltaTime
	};
}

void PhysicsSystem::Update(float deltaTime, EntityRegistry& entityRegistry) const
{
	const auto view = entityRegistry.GetEnttRegistry().view<PhysicsComponent, TransformComponent>();
	view.each([=](auto& physics, auto& transform)
	{
		// Apply forces and move the entity
		_ApplyGravity(physics);
		_TickMovement(deltaTime, physics, transform);
	});
}
