#include "Components/PhysicsComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/TransformComponent.h"
#include "EntityRegistry.h"

#include "PhysicsSystem.h"

#include <glm/geometric.hpp>

static float DEFAULT_GRAVITY = 9.8f;
static float DEFAULT_FRICTION = 0.2f;
static float MIN_VELOCITY = 0.1f;

PhysicsSystem::PhysicsSystem(EntityRegistry& entityRegistry) : mEntityRegistry(entityRegistry)
{
}

static void _ApplyForce(PhysicsComponent& physics, glm::vec3 direction)
{
	physics.mAcceleration = direction / physics.mMass;
}

static void _ProcessPlayerInput(PhysicsComponent& physics, const PlayerComponent& player)
{
	glm::vec3 direction = {0, 0, 0};
	const bool leftPressed = player.mMovementInput & MovementInput::Left;
	const bool rightPressed = player.mMovementInput & MovementInput::Right;
	const bool upPressed = player.mMovementInput & MovementInput::Up;
	const bool downPressed = player.mMovementInput & MovementInput::Down;
	const bool forwardPressed = player.mMovementInput & MovementInput::Forward;
	const bool backwardPressed = player.mMovementInput & MovementInput::Backward;

	direction.x += physics.mAccelerationRate * static_cast<float>(rightPressed - leftPressed);
	direction.y += physics.mAccelerationRate * static_cast<float>(downPressed - upPressed);
	direction.z += physics.mAccelerationRate * static_cast<float>(backwardPressed - forwardPressed);

	_ApplyForce(physics, direction);
}

static void _ApplyGravity(PhysicsComponent& physics)
{
	physics.mAcceleration.y -= DEFAULT_GRAVITY;
}

static void _ApplyFriction(PhysicsComponent& physics)
{
	physics.mVelocity *= (1.0f - DEFAULT_FRICTION);
}

static void _UpdateVelocity(float deltaTime, PhysicsComponent& physics)
{
	// Update velocity based on acceleration and time step
	physics.mVelocity.x += physics.mAcceleration.x * deltaTime;
	physics.mVelocity.y += physics.mAcceleration.y * deltaTime;
	physics.mVelocity.z += physics.mAcceleration.z * deltaTime;

	// Limit the speed to the maximum allowed speed
	const auto currentSpeed = static_cast<float>(sqrt(
		pow(physics.mVelocity.x, 2) + pow(physics.mVelocity.y, 2) + pow(physics.mVelocity.z, 2)));
	if (currentSpeed > physics.mMaxSpeed)
	{
		const float scale = physics.mMaxSpeed / currentSpeed;
		physics.mVelocity.x *= scale;
		physics.mVelocity.y *= scale;
		physics.mVelocity.z *= scale;
	}

	if (glm::length(physics.mVelocity) < MIN_VELOCITY)
	{
		// Once the entity slows down enough, we can just 0 out the velocity
		physics.mVelocity = {0, 0, 0};
	}
}

static void _TickMovement(float deltaTime, const PhysicsComponent& physics, TransformComponent& transform)
{
	// Update position based on velocity and time step
	transform.mPosition.x += physics.mVelocity.x * deltaTime;
	transform.mPosition.y += physics.mVelocity.y * deltaTime;
	transform.mPosition.z += physics.mVelocity.z * deltaTime;
}

void PhysicsSystem::Update(float deltaTime)
{
	const auto playerView = mEntityRegistry.GetEnttRegistry().view<PhysicsComponent, PlayerComponent>();
	playerView.each(_ProcessPlayerInput);

	const auto view = mEntityRegistry.GetEnttRegistry().view<PhysicsComponent, TransformComponent>();
	view.each([=](auto& physics, auto& transform)
	{
		// Apply forces and move the entity
		_ApplyGravity(physics);
		_ApplyFriction(physics);
		_UpdateVelocity(deltaTime, physics);
		_TickMovement(deltaTime, physics, transform);

		// TODO: Try Updating Collision System separately 
	});
}
