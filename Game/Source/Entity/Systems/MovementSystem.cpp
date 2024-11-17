#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

#include "Core/Camera.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/AnimationComponent.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/PlayerComponent.h"
#include "Physics/PhysicsSystem.h"

#include "MovementSystem.h"

glm::vec3 rotateAxis(0.0f, 1.0f, 0.0f);

void MovementSystem::Update(EntityRegistry& registry, PhysicsSystem& physicsSystem, const Camera& camera)
{
	const auto playerView = registry.GetEnttRegistry().view<
		PlayerComponent, AnimationComponent, PhysicsComponent, CollisionComponent>();
	playerView.each([=, &physicsSystem, &camera](const auto& player, auto& anim, auto& physics, const auto& collider)
	{
		_ApplyJump(collider, player, physicsSystem);
		// Apply movement directions to physics component
		_ApplyVelocityFromDirection(player, physics, camera);
		_ApplyMovementForce(physics, collider);

		_UpdateMovementAnim(anim, physics);
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
			auto upwardImpulse = btVector3(0, 6.0f, 0);
			collider.mRigidBody->applyCentralImpulse(upwardImpulse);
		}
	}
}

static void _ApplyVelocityFromDirection(const PlayerComponent& player, PhysicsComponent& physics, const Camera& camera)
{
	glm::vec3 velocity(0.0f, 0.0f, 0.0f);
	if (player.mMovementInput & MovementInput::Forward)
	{
		velocity += glm::normalize(camera.GetFront());
	}
	if (player.mMovementInput & MovementInput::Backward)
	{
		velocity -= glm::normalize(camera.GetFront());
	}
	if (player.mMovementInput & MovementInput::Left)
	{
		velocity -= camera.GetRight();
	}
	if (player.mMovementInput & MovementInput::Right)
	{
		velocity += camera.GetRight();
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

		btTransform transform = collider.mRigidBody->getWorldTransform();
		glm::vec3 direction = glm::normalize(glm::vec3(velocity.x, 0.0f, velocity.z));
		float yaw = std::atan2(direction.x, direction.z);

		auto targetRotation = btQuaternion({0.0f, 1.0f, 0.0f}, yaw);
		auto newRotation = transform.getRotation().slerp(targetRotation, 0.1f);
		transform.setRotation(newRotation);

		collider.mRigidBody->activate();
		collider.mRigidBody->applyCentralForce(bulletVelocity);
		collider.mRigidBody->setWorldTransform(transform);
	}
}

static void _UpdateMovementAnim(AnimationComponent& anim, const PhysicsComponent& physics)
{
	if (glm::length(physics.mVelocity) > 0.0f)
	{
		anim.TryTriggerAnimation(AnimationStates::WALKING);
	}
	else
	{
		anim.TryTriggerAnimation(AnimationStates::IDLE);
	}
}
