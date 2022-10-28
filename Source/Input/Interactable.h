#pragma once
#include <glm/vec2.hpp>
#include "Core/ResourceManager.h"
#include "UIInputManager.h"

class Interactable
{
public:
	Interactable(glm::vec2 pos, glm::vec2 size)
	{
		minX = static_cast<int>(pos.x);
		maxX = static_cast<int>(pos.x + (size.x * 2));
		minY = static_cast<int>(pos.y);
		maxY = static_cast<int>(pos.y + (size.y * 2));

		ResourceManager::GetInstance()->CreateLine({minX, 0, 1}, {minX, 400, 1}, {255, 0, 0});
		ResourceManager::GetInstance()->CreateLine({maxX, 0, 1}, {maxX, 400, 1}, {255, 0, 0});
		ResourceManager::GetInstance()->CreateLine({0, minY, 1}, {400, minY, 1}, {255, 0, 0});
		ResourceManager::GetInstance()->CreateLine({0, maxY, 1}, {400, maxY, 1}, {255, 0, 0});

		UIInputManager::RegisterInteractable(this);
	}

	virtual ~Interactable() = default;

	void OnMouseEnter()
	{
		std::cout << "OnMouseEnter" << std::endl;
		hasMouseEntered = true;
	}

	void OnMouseLeave()
	{
		std::cout << "OnMouseLeave" << std::endl;
		hasMouseEntered = false;
	}

	void HandleOnClick()
	{
		if (hasMouseEntered)
		{
			std::cout << "HandleOnClick" << std::endl;
			OnClick();
		}
	}

	virtual void OnClick() = 0;

	friend class UIInputManager;

private:
	int minX;
	int maxX;
	int minY;
	int maxY;

	bool hasMouseEntered = false;
};
