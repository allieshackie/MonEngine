#pragma once

#include <imgui.h>
#include <glm/vec2.hpp>

#include "GUIElement.h"

class GUIButton : public GUIElement
{
public:
	GUIButton(const char* label, glm::vec2 pos, glm::vec2 size, std::function<void()> clickCallback) : mLabel(label),
		mPosition(pos), mSize(size), mClickCallback(std::move(clickCallback))
	{
	}

	void Render() override
	{
		ImGui::SetCursorPos({mPosition.x, mPosition.y});
		if (ImGui::Button(mLabel, {mSize.x, mSize.y}))
		{
			if (mClickCallback)
			{
				mClickCallback();
			}
		}
	}

private:
	const char* mLabel;
	glm::vec2 mPosition = {0, 0};
	glm::vec2 mSize = {0, 0};

	std::function<void()> mClickCallback;
};
