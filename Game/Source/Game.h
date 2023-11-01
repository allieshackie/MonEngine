#pragma once
#include "Core/GameInterface.h"

class Game : public GameInterface
{
public:
	Game() = default;
	~Game() override = default;

	// Since we define a destructor, rule of 5 states that we should define
	// all copy and move constructors/operators
	// Deleting to simplify ownership
	Game(Game& other) = delete;
	Game& operator=(Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;

	void Init(EngineContext* engine) override;
	void Update(float dt) const override;
	void Render() override;
	void RegisterEntityDescriptions() const override;

private:
	EngineContext* mEngine = nullptr;
};
