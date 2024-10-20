#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

#include "Entity/EntityRegistry.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/PlayerComponent.h"
#include "Physics/PhysicsSystem.h"

#include "MovementSystem.h"

void MovementSystem::Update(EntityRegistry& registry, PhysicsSystem& physicsSystem)
{
	const auto playerView = registry.GetEnttRegistry().view<
		PlayerComponent, MeshComponent, PhysicsComponent, CollisionComponent>();
	playerView.each([=, &physicsSystem](const auto& player, auto& mesh, auto& physics, const auto& collider)
	{
		_ApplyJump(collider, player, physicsSystem);
		// Apply movement directions to physics component
		_ApplyVelocityFromDirection(player, physics);
		_ApplyMovementForce(physics, collider);

		_UpdateMovementAnim(mesh, physics);
	});
}

static void _ApplyJump(const CollisionComponent& collider, const PlayerComponent& player, PhysicsSystem& physicsSystem)
{
	if (player.mMovementInput & MovementInput::Jump)
	{
		// Check if player is on the ground (no double jumping yet)
		btVector3 start = collider.mRigidBody->getWorldTransform().getOrigin();
		btVector3 end = start - btVector3(0, 1.0, 0); // 1.0 units down

		btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
		physicsSystem.GetDynamicWorld().rayTest(start, end, rayCallback);

		if (rayCallback.hasHit())
		{
			auto upwardImpulse = btVector3(0, 0.3f, 0);
			collider.mRigidBody->applyCentralImpulse(upwardImpulse);
		}
	}
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

static void _UpdateMovementAnim(MeshComponent& mesh, const PhysicsComponent& physics)
{
	if (glm::length(physics.mVelocity) > 0.0f)
	{
		if (mesh.mCurrentAnimState != AnimationStates::WALKING)
		{
			mesh.mPrevAnimState = mesh.mCurrentAnimState;
			mesh.mCurrentAnimState = AnimationStates::WALKING;
			mesh.mBlendFactor = 0.0f;
		}
	}
	else
	{
		if (mesh.mCurrentAnimState != AnimationStates::IDLE)
		{
			mesh.mPrevAnimState = mesh.mCurrentAnimState;
			mesh.mCurrentAnimState = AnimationStates::IDLE;
			mesh.mBlendFactor = 0.0f;
		}
	}
}
