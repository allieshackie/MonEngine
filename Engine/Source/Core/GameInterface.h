#pragma once

class EngineContext;
class MonScene;

class GameInterface
{
public:
	GameInterface() = default;
	virtual ~GameInterface() = default;

	// Since we define a destructor, rule of 5 states that we should define
	// all copy and move constructors/operators
	// Deleting to simplify ownership
	GameInterface(GameInterface& other) = delete;
	GameInterface& operator=(GameInterface& other) = delete;
	GameInterface(GameInterface&& other) = delete;
	GameInterface& operator=(GameInterface&& other) = delete;

	// called when the engine is ready for the game to initialize
	virtual void Init(EngineContext* engine) = 0;

	// run the simulation
	virtual void Update(float dt) const = 0;
	virtual void Render() = 0;

	virtual void RegisterEntityDescriptions() const = 0;
	virtual void SetSceneCallbacks(const MonScene* scene) const = 0;
};
