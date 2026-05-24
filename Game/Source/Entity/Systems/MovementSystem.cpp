#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

#include "Core/Camera.h"
#include "Core/World.h"
#include "Entity/Components/AnimationComponent.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/PlayerComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Physics/PhysicsSystem.h"

#include "MovementSystem.h"

glm::vec3 rotateAxis(0.0f, 1.0f, 0.0f);
float speed = 5.0f;

MovementSystem::MovementSystem(PhysicsSystem& physicsSystem, std::weak_ptr<World> world)
	: mPhysicsSystem(physicsSystem), mWorld(world)
{
}

void MovementSystem::Update(float dt)
{
	if (const auto world = mWorld.lock())
	{
		const auto playerView = world->GetRegistry().view<
			PlayerComponent, AnimationComponent, PhysicsComponent, CollisionComponent, TransformComponent>();
		playerView.each(
			[=](auto& player, auto& anim, auto& physics, const auto& collider, auto& transform)
			{
				_ApplyJump(collider, player, mPhysicsSystem);
				_ApplyVelocityFromDirection(player, physics, world->GetCamera());
				_ApplyMovementForce(physics, collider, transform);
				_UpdateMovementAnim(anim, physics);
			});
	}
}

static void _ApplyJump(const CollisionComponent& collider, PlayerComponent& player, PhysicsSystem& physicsSystem)
{
	if (!(player.mMovementInput & MovementInput::Jump))
	{
		return;
	}

	// Consume the input immediately so it only fires once per press
	player.mMovementInput &= ~MovementInput::Jump;
	// Check if player is on the ground (no double jumping yet)
	btVector3 start = collider.mRigidBody->getWorldTransform().getOrigin();
	btVector3 end = start - btVector3(0, 3.0, 0);

	btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
	physicsSystem.GetDynamicWorld().rayTest(start, end, rayCallback);

	if (rayCallback.hasHit())
	{
		auto upwardImpulse = btVector3(0, 10.0f, 0);
		collider.mRigidBody->applyCentralImpulse(upwardImpulse);
	}
}

static void _ApplyVelocityFromDirection(const PlayerComponent& player, PhysicsComponent& physics, const Camera& camera)
{
	glm::vec3 velocity(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraForward = glm::normalize(camera.GetFront());
	glm::vec3 cameraRight = glm::normalize(camera.GetRight());

	if (player.mMovementInput & MovementInput::Forward)
	{
		velocity += cameraForward;
	}
	if (player.mMovementInput & MovementInput::Backward)
	{
		velocity -= cameraForward;
	}
	
	if (player.mMovementInput & MovementInput::Left)
	{
		velocity += cameraRight;
	}
	if (player.mMovementInput & MovementInput::Right)
	{
		velocity -= cameraRight;
	}

	if (glm::length(velocity) > 0.0f)
	{
		velocity = glm::normalize(velocity);
	}
	physics.mVelocity = velocity;
}

static void _ApplyMovementForce(const PhysicsComponent& physics, const CollisionComponent& collider, TransformComponent& transform)
{
	if (collider.mRigidBody)
	{
		glm::vec3 moveDir = physics.mVelocity;
		if (glm::length(moveDir) < 0.1f)
		{
			return;
		}

		// Turn towards movement
		glm::vec3 flatDir = glm::normalize(glm::vec3(moveDir.x, 0.0f, moveDir.z));
		transform.mRotation.y = glm::degrees(atan2(-flatDir.x, -flatDir.z));

		// Actually move in direction
		btVector3 currentVel = collider.mRigidBody->getLinearVelocity();
		btVector3 moveVelocity(physics.mVelocity.x * speed, currentVel.y(), physics.mVelocity.z * speed);
		collider.mRigidBody->activate(true);
		collider.mRigidBody->setLinearVelocity(moveVelocity);
	}
}

static void _UpdateMovementAnim(AnimationComponent& anim, const PhysicsComponent& physics)
{
	if (glm::length(physics.mVelocity) > 0.0f)
	{
		anim.TryTriggerAnimation(2);
	}
	else
	{
		anim.TryTriggerAnimation(-1);
	}
}
