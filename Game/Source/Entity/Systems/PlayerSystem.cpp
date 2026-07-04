#include "LLGL/Key.h"

#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/PlayerComponent.h"
#include "Input/InputHandler.h"

#include "PlayerSystem.h"

PlayerSystem::PlayerSystem(std::weak_ptr<InputHandler> inputHandler, EventPublisher& eventPublisher)
	: mInputHandler(inputHandler)
{
	eventPublisher.AddWorldCreatedListener(
		[this](std::weak_ptr<World> world) {
			if (!mHandlers.empty())
			{
				if (const auto inputHandlerPtr = mInputHandler.lock())
				{
					for (auto handler : mHandlers)
					{
						inputHandlerPtr->Unregister(handler);
					}
				}
			}
			if (const auto worldPtr = world.lock())
			{
				EventFunc func = [this](Entity* entity)
					{
						SpawnPlayer(entity);
					};
				worldPtr->ConnectOnConstruct<PlayerComponent>(func);
			}
		}
	);
}

void PlayerSystem::SpawnPlayer(Entity* entity)
{
	auto& player = entity->GetComponent<PlayerComponent>();

	if (const auto inputHandler = mInputHandler.lock())
	{
		// Forward
		HandlerId wDownHandler = inputHandler->RegisterButtonDownHandler(LLGL::Key::W, [&player]()
		{
			player.mMovementInput |= MovementInput::Forward;
		});

		HandlerId wUpHandler = inputHandler->RegisterButtonUpHandler(LLGL::Key::W, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Forward);
		});
		

		// Back
		HandlerId sDownHandler = inputHandler->RegisterButtonDownHandler(LLGL::Key::S, [&player]()
		{
			player.mMovementInput |= MovementInput::Backward;
		});

		HandlerId sUpHandler = inputHandler->RegisterButtonUpHandler(LLGL::Key::S, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Backward);
		});

		// Left
		HandlerId aDownHandler = inputHandler->RegisterButtonDownHandler(LLGL::Key::A, [&player]()
		{
			player.mMovementInput |= MovementInput::Left;
		});

		HandlerId aUpHandler = inputHandler->RegisterButtonUpHandler(LLGL::Key::A, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Left);
		});

		// Right
		HandlerId dDownHandler = inputHandler->RegisterButtonDownHandler(LLGL::Key::D, [&player]()
		{
			player.mMovementInput |= MovementInput::Right;
		});

		HandlerId dUpHandler = inputHandler->RegisterButtonUpHandler(LLGL::Key::D, [&player]()
		{
			player.mMovementInput &= ~(MovementInput::Right);
		});

		// Jump
		HandlerId jumpHandler = inputHandler->RegisterButtonDownHandler(LLGL::Key::Space, [&player]()
		{
			player.mMovementInput |= MovementInput::Jump;
		});

		mHandlers.push_back(wDownHandler);
		mHandlers.push_back(wUpHandler);
		mHandlers.push_back(sDownHandler);
		mHandlers.push_back(sUpHandler);
		mHandlers.push_back(aDownHandler);
		mHandlers.push_back(aUpHandler);
		mHandlers.push_back(dDownHandler);
		mHandlers.push_back(dUpHandler);
		mHandlers.push_back(jumpHandler);
	}
}
