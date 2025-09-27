#include "LLGL/Key.h"

#include "Core/SceneManager.h"
#include "Entity/Entity.h"
#include "Entity/Components/PlayerComponent.h"
#include "Input/InputHandler.h"

#include "PlayerSystem.h"

PlayerSystem::PlayerSystem(InputHandler& inputHandler)
	: mInputHandler(inputHandler)
{
}

void PlayerSystem::SpawnPlayer(Entity* entity) const
{
	auto& player = entity->GetComponent<PlayerComponent>();

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

	// Jump
	mInputHandler.RegisterButtonDownHandler(LLGL::Key::Space, [&player]()
	{
		player.mMovementInput |= MovementInput::Jump;
	});

	mInputHandler.RegisterButtonUpHandler(LLGL::Key::Space, [&player]()
	{
		player.mMovementInput &= ~(MovementInput::Jump);
	});
}

void PlayerSystem::SetSceneCallbacks(const SceneManager& sceneManager) const
{
	EventFunc func = [this](Entity* entity)
	{
		SpawnPlayer(entity);
	};
	sceneManager.ConnectOnConstruct<PlayerComponent>(func);
}
