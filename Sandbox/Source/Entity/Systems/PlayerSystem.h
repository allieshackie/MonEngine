#pragma once

class Entity;
class InputHandler;
class World;

class PlayerSystem
{
public:
	PlayerSystem(InputHandler& inputHandler);

	void SpawnPlayer(Entity* entity) const;

	void SetSceneCallbacks(World* world) const;

private:
	InputHandler& mInputHandler;
};
