#include "Interactable.h"
#include "InputManager.h"

#include "UIInputManager.h"

std::vector<Interactable*> UIInputManager::mInteractables;

UIInputManager::UIInputManager(InputManager& inputManager)
{
	inputManager.registerMouseMoveHandler([this](LLGL::Offset2D mousePos) { Update(mousePos); });
	inputManager.registerButtonDownHandler(LLGL::Key::LButton, [this]() { HandleOnClick(); });
}

void UIInputManager::RegisterInteractable(Interactable* interactable)
{
	mInteractables.push_back(std::move(interactable));
}

void UIInputManager::Update(LLGL::Offset2D mousePos)
{
	for (auto interactable : mInteractables)
	{
		if (mousePos.x > interactable->minX && mousePos.x < interactable->maxX && mousePos.y > interactable->minY && mousePos.y < interactable->maxY)
		{
			if (!interactable->hasMouseEntered)
			{
				interactable->OnMouseEnter();
			}
		}
		else
		{
			if (interactable->hasMouseEntered)
			{
				interactable->OnMouseLeave();
			}
		}
	}
}

void UIInputManager::HandleOnClick()
{
	for (auto interactable : mInteractables)
	{
		if (interactable->hasMouseEntered)
		{
			interactable->HandleOnClick();
		}
	}
}
