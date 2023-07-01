#include "LLGL/Key.h"
#include "Entity/EntityRegistry.h"
#include "Input/InputManager.h"
#include "Entity/Components/PlayerComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "PlayerSystem.h"


PlayerSystem::PlayerSystem(EntityRegistry& entityRegistry, InputManager& inputManager)
	: mEntityRegistry(entityRegistry), mInputManager(inputManager)
{
}

void PlayerSystem::SpawnPlayer(glm::vec3 spawnPoint)
{
	mPlayerEntity = mEntityRegistry.CreateEntityFromTemplate("player");

	const auto playerView = mEntityRegistry.GetEnttRegistry().view<const PlayerComponent, TransformComponent>();
	for (const auto& entity : playerView)
	{
		auto& transform = playerView.get<TransformComponent>(entity);
		transform.mPosition = spawnPoint;
	}

	mInputManager.registerButtonDownHandler(LLGL::Key::Up, [=]()
	{
		_MoveForward(true);
	});
	mInputManager.registerButtonUpHandler(LLGL::Key::Up, [=]()
	{
		_MoveForward(false);
	});

	mInputManager.registerButtonDownHandler(LLGL::Key::Down, [=]()
	{
		_MoveBackward(true);
	});
	mInputManager.registerButtonUpHandler(LLGL::Key::Down, [=]()
	{
		_MoveBackward(false);
	});

	mInputManager.registerButtonDownHandler(LLGL::Key::Left, [=]()
	{
		_MoveLeft(true);
	});
	mInputManager.registerButtonUpHandler(LLGL::Key::Left, [=]()
	{
		_MoveLeft(false);
	});

	mInputManager.registerButtonDownHandler(LLGL::Key::Right, [=]()
	{
		_MoveRight(true);
	});
	mInputManager.registerButtonUpHandler(LLGL::Key::Right, [=]()
	{
		_MoveRight(false);
	});
}

glm::vec3 PlayerSystem::GetPlayerPosition() const
{
	glm::vec3 position = {0, 0, 0};
	const auto playerView = mEntityRegistry.GetEnttRegistry().view<const PlayerComponent, TransformComponent>();
	for (const auto& entity : playerView)
	{
		const auto& transform = playerView.get<TransformComponent>(entity);
		position = transform.mPosition;
	}

	return position;
}

void PlayerSystem::SetPlayerPosition(glm::vec3 newPosition) const
{
	const auto playerView = mEntityRegistry.GetEnttRegistry().view<const PlayerComponent, TransformComponent>();
	for (const auto& entity : playerView)
	{
		auto& transform = playerView.get<TransformComponent>(entity);
		transform.mPosition = newPosition;
	}
}

void PlayerSystem::_MoveForward(bool set) const
{
	auto& playerComponent = mEntityRegistry.GetComponent<PlayerComponent>(mPlayerEntity);
	if (set) playerComponent.mMovementInput |= MovementInput::Forward;
	else playerComponent.mMovementInput &= ~MovementInput::Forward;
}

void PlayerSystem::_MoveBackward(bool set) const
{
	auto& playerComponent = mEntityRegistry.GetComponent<PlayerComponent>(mPlayerEntity);
	if (set) playerComponent.mMovementInput |= MovementInput::Backward;
	else playerComponent.mMovementInput &= ~MovementInput::Backward;
}

void PlayerSystem::_MoveLeft(bool set) const
{
	auto& playerComponent = mEntityRegistry.GetComponent<PlayerComponent>(mPlayerEntity);
	if (set) playerComponent.mMovementInput |= MovementInput::Left;
	else playerComponent.mMovementInput &= ~MovementInput::Left;
}

void PlayerSystem::_MoveRight(bool set) const
{
	auto& playerComponent = mEntityRegistry.GetComponent<PlayerComponent>(mPlayerEntity);
	if (set) playerComponent.mMovementInput |= MovementInput::Right;
	else playerComponent.mMovementInput &= ~MovementInput::Right;
}
