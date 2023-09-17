#include "Entity/EntityRegistry.h"
#include "Entity/Components/PlayerComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "PlayerSystem.h"

PlayerSystem::PlayerSystem(EntityRegistry& entityRegistry)
	: mEntityRegistry(entityRegistry)
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

	/*
	 *
	mInputHandler.RegisterButtonDownHandler(LLGL::Key::Up, [=]()
	{
		_MoveForward(true);
	});
	mInputHandler.RegisterButtonUpHandler(LLGL::Key::Up, [=]()
	{
		_MoveForward(false);
	});

	mInputHandler.RegisterButtonDownHandler(LLGL::Key::Down, [=]()
	{
		_MoveBackward(true);
	});
	mInputHandler.RegisterButtonUpHandler(LLGL::Key::Down, [=]()
	{
		_MoveBackward(false);
	});

	mInputHandler.RegisterButtonDownHandler(LLGL::Key::Left, [=]()
	{
		_MoveLeft(true);
	});
	mInputHandler.RegisterButtonUpHandler(LLGL::Key::Left, [=]()
	{
		_MoveLeft(false);
	});

	mInputHandler.RegisterButtonDownHandler(LLGL::Key::Right, [=]()
	{
		_MoveRight(true);
	});
	mInputHandler.RegisterButtonUpHandler(LLGL::Key::Right, [=]()
	{
		_MoveRight(false);
	});
	 */
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
