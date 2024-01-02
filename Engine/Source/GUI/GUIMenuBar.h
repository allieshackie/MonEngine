#pragma once
#include <imgui.h>

#include "GUIMenuBase.h"
#include "GUIMenuBarItem.h"

class GUIMenuBar : public GUIMenuBase
{
public:
	void Render() override
	{
		if (ImGui::BeginMainMenuBar())
		{
			Impl_Render();

			ImGui::EndMainMenuBar();
		}
	}

	void AddMenuItem(const char* label, std::function<void()> clickCallback)
	{
		auto item = std::make_unique<GUIMenuBarItem>(label, std::move(clickCallback));
		AddElement(std::move(item));
	}
};
