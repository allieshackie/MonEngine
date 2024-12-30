#pragma once

class Entity;
class InputHandler;
class SceneManager;

class PlayerSystem
{
public:
	PlayerSystem(InputHandler& inputHandler);

	void SpawnPlayer(Entity* entity) const;

	void SetSceneCallbacks(const SceneManager& sceneManager) const;

private:
	InputHandler& mInputHandler;
};
