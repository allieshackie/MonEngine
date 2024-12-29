#pragma once

class Entity;
class InputHandler;
class MonScene;

class PlayerSystem
{
public:
	PlayerSystem(InputHandler& inputHandler);

	void SpawnPlayer(Entity* entity) const;

	void SetSceneCallbacks(const MonScene* scene) const;

private:
	InputHandler& mInputHandler;
};
