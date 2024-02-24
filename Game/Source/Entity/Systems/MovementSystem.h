#pragma once

class EntityRegistry;
class InputHandler;

class MovementSystem
{
public:
	void Update(InputHandler& inputHandler, EntityRegistry& registry);
};
