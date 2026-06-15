#pragma once
#include "Core/ISystem.h"

class Entity;
class EventPublisher;
class InputHandler;
class World;

class PlayerSystem : public ISystem
{
public:
	PlayerSystem(std::weak_ptr<InputHandler> inputHandler, EventPublisher& eventPublisher);

	void SpawnPlayer(Entity* entity) const;

private:
	std::weak_ptr<InputHandler> mInputHandler;
};
