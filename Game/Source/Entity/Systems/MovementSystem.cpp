#include <glm/vec3.hpp>

#include "Entity/EntityRegistry.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/PlayerComponent.h"
#include "Input/InputHandler.h"

#include "MovementSystem.h"

void MovementSystem::Update(InputHandler& inputHandler, EntityRegistry& registry)
{
	const auto playerView = registry.GetEnttRegistry().view<PlayerComponent>();
	playerView.each([=, &inputHandler, &registry](auto entity, auto& player)
	{
		if (!player.mRegistered)
		{
			_RegisterPlayerInputBindings(inputHandler, registry, entity);
			player.mRegistered = true;
		}
	});
}

static void _RegisterPlayerInputBindings(InputHandler& inputHandler, EntityRegistry& registry, EntityId entity)
{
	// Forward
	inputHandler.RegisterButtonDownHandler(LLGL::Key::W, [=, &registry]()
	{
		auto& player = registry.GetComponent<PlayerComponent>(entity);
		player.mMovementInput |= MovementInput::Forward;
	});

	inputHandler.RegisterButtonUpHandler(LLGL::Key::W, [=, &registry]()
	{
		auto& player = registry.GetComponent<PlayerComponent>(entity);
		player.mMovementInput &= ~(MovementInput::Forward);
	});

	// Back
	inputHandler.RegisterButtonDownHandler(LLGL::Key::S, [=, &registry]()
	{
		auto& player = registry.GetComponent<PlayerComponent>(entity);
		player.mMovementInput |= MovementInput::Backward;
	});

	inputHandler.RegisterButtonUpHandler(LLGL::Key::S, [=, &registry]()
	{
		auto& player = registry.GetComponent<PlayerComponent>(entity);
		player.mMovementInput &= ~(MovementInput::Backward);
	});

	// Left
	inputHandler.RegisterButtonDownHandler(LLGL::Key::A, [=, &registry]()
	{
		auto& player = registry.GetComponent<PlayerComponent>(entity);
		player.mMovementInput |= MovementInput::Left;
	});

	inputHandler.RegisterButtonUpHandler(LLGL::Key::A, [=, &registry]()
	{
		auto& player = registry.GetComponent<PlayerComponent>(entity);
		player.mMovementInput &= ~(MovementInput::Left);
	});

	// Right
	inputHandler.RegisterButtonDownHandler(LLGL::Key::D, [=, &registry]()
	{
		auto& player = registry.GetComponent<PlayerComponent>(entity);
		player.mMovementInput |= MovementInput::Right;
	});

	inputHandler.RegisterButtonUpHandler(LLGL::Key::D, [=, &registry]()
	{
		auto& player = registry.GetComponent<PlayerComponent>(entity);
		player.mMovementInput &= ~(MovementInput::Right);
	});
}
