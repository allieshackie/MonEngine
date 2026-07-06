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

MovementSystem::MovementSystem(PhysicsSystem& physicsSystem, EventPublisher& eventPublisher)
	: mPhysicsSystem(physicsSystem)
{
	eventPublisher.AddWorldCreatedListener(
		[this](std::weak_ptr<World> world) {
			mWorld = world;
		}
	);
}

void MovementSystem::Update(float dt)
{
	if (const auto world = mWorld.lock())
	{
		const auto playerView = world->GetRegistry().view<
			PlayerComponent, AnimationComponent, PhysicsComponent, CollisionComponent, TransformComponent>();
		playerView.each(
			[=](entt::entity entityId, auto& player, auto& anim, auto& physics, const auto& collider, auto& transform)
			{
				_ApplyJump(entityId, player, mPhysicsSystem);
				_ApplyVelocityFromDirection(player, physics, world->GetCamera());
				_ApplyMovementForce(entityId, mPhysicsSystem, physics, transform);
				_UpdateMovementAnim(anim, physics);

				transform.mRotation = glm::slerp(transform.mRotation, transform.mQueuedRotation, 10.0f * dt);
			});
	}
}

static void _ApplyJump(entt::entity entityId, PlayerComponent& player, PhysicsSystem& physicsSystem)
{
	if (!(player.mMovementInput & MovementInput::Jump))
	{
		return;
	}

	// Consume the input immediately so it only fires once per press
	player.mMovementInput &= ~MovementInput::Jump;
	// Check if player is on the ground (no double jumping yet)
	auto rigidBody = physicsSystem.GetRigidbody(entityId);
	if (rigidBody)
	{
		btVector3 start = rigidBody->getWorldTransform().getOrigin();
		btVector3 end = start - btVector3(0, 3.0, 0);

		if (physicsSystem.Raycast(start,end))
		{
			auto upwardImpulse = btVector3(0, 10.0f, 0);
			rigidBody->applyCentralImpulse(upwardImpulse);
		}
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

static void _ApplyMovementForce(entt::entity entityId, PhysicsSystem& physicsSystem, const PhysicsComponent& physics, TransformComponent& transform)
{
	auto rigidBody = physicsSystem.GetRigidbody(entityId);
	if (rigidBody)
	{
		glm::vec3 moveDir = physics.mVelocity;
		if (glm::length(moveDir) < 0.1f)
		{
			return;
		}

		// Turn towards movement
		glm::vec3 flatDir = glm::normalize(glm::vec3(moveDir.x, 0.0f, moveDir.z));
		float angle = atan2(-flatDir.x, -flatDir.z); // stays in radians
		transform.mQueuedRotation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
		
		// Actually move in direction
		btVector3 currentVel = rigidBody->getLinearVelocity();
		btVector3 moveVelocity(physics.mVelocity.x * speed, currentVel.y(), physics.mVelocity.z * speed);
		rigidBody->activate(true);
		rigidBody->setLinearVelocity(moveVelocity);
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
