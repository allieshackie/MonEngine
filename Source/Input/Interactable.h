#pragma once
#include <glm/vec2.hpp>
#include "LLGL/Types.h"
#include "Core/ResourceManager.h"

#include "InputHandler.h"

class Interactable
{
public:
	Interactable(glm::vec2 pos, glm::vec2 size)
	{
		minX = static_cast<int>(pos.x);
		maxX = static_cast<int>(pos.x + (size.x * 2));
		minY = static_cast<int>(pos.y);
		maxY = static_cast<int>(pos.y + (size.y * 2));

		ResourceManager::CreateLine({minX, 0, minX, 400}, {255,0,0});
		ResourceManager::CreateLine({ maxX, 0, maxX, 400}, {255,0,0});
		ResourceManager::CreateLine({0, minY, 400, minY }, {255,0,0});
		ResourceManager::CreateLine({ 0, maxY, 400, maxY }, {255,0,0});

		InputHandler::registerMouseMoveHandler([this](LLGL::Offset2D mousePos) { Update(mousePos); });
		InputHandler::registerButtonDownHandler(LLGL::Key::LButton, [this]() { HandleOnClick(); });
	}
	virtual ~Interactable() = default;

	void Update(LLGL::Offset2D mousePos)
	{
		if (mousePos.x > minX && mousePos.x < maxX && mousePos.y > minY && mousePos.y < maxY)
		{
			if (!hasMouseEntered)
			{
				OnMouseEnter();
			}
		}
		else
		{
			if (hasMouseEntered)
			{
				OnMouseLeave();
			}
		}
	}

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

private:
	int minX;
	int maxX;
	int minY;
	int maxY;

	bool hasMouseEntered = false;
};
