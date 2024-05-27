#pragma once
#include "Entity/EntityRegistry.h"

class InputHandler;

class PlayerSystem
{
public:
	PlayerSystem(EntityRegistry& entityRegistry, InputHandler& inputHandler);

	void SpawnPlayer();

private:
	EntityRegistry& mEntityRegistry;
	InputHandler& mInputHandler;
};
