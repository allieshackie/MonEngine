#pragma once
#include <imgui.h>

#include "GUIButton.h"
#include "GUIElement.h"

class GUIMenu
{
public:
	GUIMenu(const char* label, glm::vec2 pos = {0, 0},
	        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize |
		        ImGuiWindowFlags_NoMove) : mLabel(label), mPosition(pos), mWindowFlags(windowFlags)
	{
	}

	void Render()
	{
		ImGui::SetNextWindowPos(ImVec2(mPosition.x, mPosition.y));
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		if (ImGui::Begin(mLabel, &mOpen, mWindowFlags))
		{
			for (const auto& element : mElements)
			{
				element->Render();
			}
		}
		ImGui::End();
	}

	void AddButton(const char* label, float posX, float posY, float sizeX, float sizeY,
	               std::function<void()> clickCallback)
	{
		auto button = std::make_unique<GUIButton>(label, glm::vec2{posX, posY}, glm::vec2{sizeX, sizeY},
		                                          std::move(clickCallback));
		AddElement(std::move(button));
	}

	void AddElement(std::unique_ptr<GUIElement> element)
	{
		mElements.push_back(std::move(element));
	}

private:
	bool mOpen = true;
	const char* mLabel;
	glm::vec2 mPosition = {0, 0};

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	std::vector<std::unique_ptr<GUIElement>> mElements;
};
