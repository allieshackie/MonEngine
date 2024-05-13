#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "Entity/EntityRegistry.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/PlayerComponent.h"

#include "MovementSystem.h"

void MovementSystem::Update(EntityRegistry& registry)
{
	const auto playerView = registry.GetEnttRegistry().view<PlayerComponent, PhysicsComponent, CollisionComponent>();
	playerView.each([=](auto& player, auto& physics, auto& collider)
	{
		// Apply movement directions to physics component
		_ApplyVelocityFromDirection(player, physics);
		_ApplyMovementForce(physics, collider);
	});
}

static void _ApplyVelocityFromDirection(const PlayerComponent& player, PhysicsComponent& physics)
{
	glm::vec3 velocity(0.0f, 0.0f, 0.0f);
	if (player.mMovementInput & MovementInput::Forward)
	{
		velocity.z -= 1.0f;
	}
	if (player.mMovementInput & MovementInput::Backward)
	{
		velocity.z += 1.0f;
	}
	if (player.mMovementInput & MovementInput::Left)
	{
		velocity.x -= 1.0f;
	}
	if (player.mMovementInput & MovementInput::Right)
	{
		velocity.x += 1.0f;
	}

	if (glm::length(velocity) > 0.0f)
	{
		velocity = glm::normalize(velocity);
	}
	physics.mVelocity = velocity;
}

static void _ApplyMovementForce(const PhysicsComponent& physics, const CollisionComponent& collider)
{
	if (collider.mRigidBody)
	{
		const glm::vec3& velocity = physics.mVelocity;
		if (glm::all(glm::equal(velocity, glm::vec3(0.0f))))
		{
			return;
		}

		// Apply a force to the player's rigid body based on the calculated velocity
		btVector3 bulletVelocity(velocity.x, velocity.y, velocity.z);
		bulletVelocity *= 10.0f; // Multiply by a speed factor

		//collider.mRigidBody->applyCentralImpulse(bulletVelocity);
		collider.mRigidBody->applyCentralForce(bulletVelocity);
	}
}
