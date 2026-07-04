#pragma once
#include "Core/ISystem.h"

class Entity;
class EventPublisher;
class InputHandler;
class World;

using HandlerId = size_t;

class PlayerSystem : public ISystem
{
public:
	PlayerSystem(std::weak_ptr<InputHandler> inputHandler, EventPublisher& eventPublisher);

	void SpawnPlayer(Entity* entity);

private:
	std::weak_ptr<InputHandler> mInputHandler;
	std::vector<HandlerId> mHandlers;
};
