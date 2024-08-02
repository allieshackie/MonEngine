#pragma once
#include "Entity/EntityRegistry.h"

class InputHandler;

class PlayerSystem
{
public:
	PlayerSystem(EntityRegistry& entityRegistry, InputHandler& inputHandler);

	void SpawnPlayer(EnTTRegistry& registry, EntityId entity);

private:
	EntityRegistry& mEntityRegistry;
	InputHandler& mInputHandler;
};
