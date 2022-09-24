#pragma once
#include "LLGL/Types.h"

class InputManager;
class Interactable;

class UIInputManager
{
public:

	UIInputManager(InputManager& inputManager);
	~UIInputManager() = default;

	static void RegisterInteractable(Interactable* interactable);

	void Update(LLGL::Offset2D mousePos);
	void HandleOnClick();

private:
	static std::vector<Interactable*> mInteractables;
};