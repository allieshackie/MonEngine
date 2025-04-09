#pragma once
#include "Core/GameInterface.h"

class App : public GameInterface
{
public:
	App() = default;
	~App() override = default;

	// Since we define a destructor, rule of 5 states that we should define
	// all copy and move constructors/operators
	// Deleting to simplify ownership
	App(App& other) = delete;
	App& operator=(App& other) = delete;
	App(App&& other) = delete;
	App& operator=(App&& other) = delete;

	void Init(EngineContext* engine) override;
	void StartGame() override;
	void Update(float dt) const override;
	void Render() override;
	void RegisterEntityDescriptions() const override;
	void SetSceneCallbacks(const SceneManager& sceneManager) const override;

private:
	EngineContext* mEngine = nullptr;
};
