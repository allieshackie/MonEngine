#pragma once
#include "Core/ISystem.h"

class Entity;
class InputHandler;
class World;

class PlayerSystem : public ISystem
{
public:
	PlayerSystem(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world);

	void SpawnPlayer(Entity* entity) const;

private:
	std::weak_ptr<InputHandler> mInputHandler;
};
