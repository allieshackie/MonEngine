#include "LLGL/Key.h"

#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/PlayerComponent.h"
#include "Input/InputHandler.h"

#include "PlayerSystem.h"

PlayerSystem::PlayerSystem(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world)
	: mInputHandler(inputHandler)
{
	if (auto worldPtr = world.lock())
	{
		EventFunc func = [this](Entity* entity)
		{
			SpawnPlayer(entity);
		};
		worldPtr->ConnectOnConstruct<PlayerComponent>(func);
	}
}

void PlayerSystem::SpawnPlayer(Entity* entity) const
{
	auto& player = entity->GetComponent<PlayerComponent>();

	if (const auto inputHandler = mInputHandler.lock())
	{
		// Forward
		inputHandler->RegisterButtonDownHandler(LLGL::Key::W, [&player]()
		{
			player.mMovementInput |= MovementInput::Forward;
		});

		inputHandler->RegisterButtonUpHandler(LLGL::Key::W, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Forward);
		});

		// Back
		inputHandler->RegisterButtonDownHandler(LLGL::Key::S, [&player]()
		{
			player.mMovementInput |= MovementInput::Backward;
		});

		inputHandler->RegisterButtonUpHandler(LLGL::Key::S, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Backward);
		});

		// Left
		inputHandler->RegisterButtonDownHandler(LLGL::Key::A, [&player]()
		{
			player.mMovementInput |= MovementInput::Left;
		});

		inputHandler->RegisterButtonUpHandler(LLGL::Key::A, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Left);
		});

		// Right
		inputHandler->RegisterButtonDownHandler(LLGL::Key::D, [&player]()
		{
			player.mMovementInput |= MovementInput::Right;
		});

		inputHandler->RegisterButtonUpHandler(LLGL::Key::D, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Right);
		});

		// Jump
		inputHandler->RegisterButtonDownHandler(LLGL::Key::Space, [&player]()
		{
			player.mMovementInput |= MovementInput::Jump;
		});

		inputHandler->RegisterButtonUpHandler(LLGL::Key::Space, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Jump);
		});
	}
}
