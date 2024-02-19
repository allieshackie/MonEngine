#pragma once
#include <imgui.h>

#include "GUIElement.h"

class GUIMenuBarItem : public GUIElement
{
public:
	GUIMenuBarItem(const char* label, std::function<void()> clickCallback)
		: mLabel(label), mClickCallback(std::move(clickCallback))
	{
	}

	void Render() override
	{
		if (ImGui::MenuItem(mLabel))
		{
			if (mClickCallback)
			{
				mClickCallback();
			}
		}
	}

private:
	const char* mLabel;
	std::function<void()> mClickCallback;
};
