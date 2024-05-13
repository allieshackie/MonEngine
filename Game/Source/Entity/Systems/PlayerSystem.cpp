#include "LLGL/Key.h"

#include "Entity/EntityRegistry.h"
#include "Entity/Components/PlayerComponent.h"
#include "Input/InputHandler.h"

#include "PlayerSystem.h"

PlayerSystem::PlayerSystem(EntityRegistry& entityRegistry, InputHandler& inputHandler)
	: mEntityRegistry(entityRegistry), mInputHandler(inputHandler)
{
	mEntityRegistry.GetEnttRegistry().on_construct<PlayerComponent>().connect<&PlayerSystem::SpawnPlayer>(this);
}

void PlayerSystem::SpawnPlayer()
{
	const auto view = mEntityRegistry.GetEnttRegistry().view<PlayerComponent>();
	view.each([=](auto& player)
	{
		// Forward
		mInputHandler.RegisterButtonDownHandler(LLGL::Key::W, [&player]()
		{
			player.mMovementInput |= MovementInput::Forward;
		});

		mInputHandler.RegisterButtonUpHandler(LLGL::Key::W, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Forward);
		});

		// Back
		mInputHandler.RegisterButtonDownHandler(LLGL::Key::S, [&player]()
		{
			player.mMovementInput |= MovementInput::Backward;
		});

		mInputHandler.RegisterButtonUpHandler(LLGL::Key::S, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Backward);
		});

		// Left
		mInputHandler.RegisterButtonDownHandler(LLGL::Key::A, [&player]()
		{
			player.mMovementInput |= MovementInput::Left;
		});

		mInputHandler.RegisterButtonUpHandler(LLGL::Key::A, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Left);
		});

		// Right
		mInputHandler.RegisterButtonDownHandler(LLGL::Key::D, [&player]()
		{
			player.mMovementInput |= MovementInput::Right;
		});

		mInputHandler.RegisterButtonUpHandler(LLGL::Key::D, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Right);
		});
	});
}
