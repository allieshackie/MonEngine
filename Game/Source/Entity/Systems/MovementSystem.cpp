#include <glm/vec3.hpp>

#include "Entity/EntityRegistry.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/PlayerComponent.h"
#include "Input/InputHandler.h"

#include "MovementSystem.h"

void MovementSystem::Update(InputHandler& inputHandler, EntityRegistry& registry)
{
	const auto playerView = registry.GetEnttRegistry().view<PlayerComponent, PhysicsComponent>();
	playerView.each([=, &inputHandler](auto& player, auto& physics)
	{
		// Register input for player 
		if (!player.mRegistered)
		{
			_RegisterPlayerInputBindings(inputHandler, player);
			player.mRegistered = true;
		}
		// Apply movement directions to physics component
		else
		{
			_ApplyVelocityFromDirection(player, physics);
		}
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

	physics.mVelocity = velocity;
}

static void _RegisterPlayerInputBindings(InputHandler& inputHandler, PlayerComponent& player)
{
	// Forward
	inputHandler.RegisterButtonDownHandler(LLGL::Key::W, [&player]()
	{
		player.mMovementInput |= MovementInput::Forward;
	});

	inputHandler.RegisterButtonUpHandler(LLGL::Key::W, [&player]()
	{
		player.mMovementInput &= ~(MovementInput::Forward);
	});

	// Back
	inputHandler.RegisterButtonDownHandler(LLGL::Key::S, [&player]()
	{
		player.mMovementInput |= MovementInput::Backward;
	});

	inputHandler.RegisterButtonUpHandler(LLGL::Key::S, [&player]()
	{
		player.mMovementInput &= ~(MovementInput::Backward);
	});

	// Left
	inputHandler.RegisterButtonDownHandler(LLGL::Key::A, [&player]()
	{
		player.mMovementInput |= MovementInput::Left;
	});

	inputHandler.RegisterButtonUpHandler(LLGL::Key::A, [&player]()
	{
		player.mMovementInput &= ~(MovementInput::Left);
	});

	// Right
	inputHandler.RegisterButtonDownHandler(LLGL::Key::D, [&player]()
	{
		player.mMovementInput |= MovementInput::Right;
	});

	inputHandler.RegisterButtonUpHandler(LLGL::Key::D, [&player]()
	{
		player.mMovementInput &= ~(MovementInput::Right);
	});
}
