#pragma once
#include <imgui.h>
#include <vector>
#include <memory>
#include <glm/vec2.hpp>

#include "GUIElement.h"

class GUIMenu
{
public:
	GUIMenu(std::string label, glm::vec2 pos, glm::vec2 size);

	void Render();
	void AddElement(std::unique_ptr<GUIElement> element);

private:
	std::vector<std::unique_ptr<GUIElement>> mElements;

	bool mOpen = true;
	std::string mLabel;
	ImVec2 mPosition = {0, 0};
	ImVec2 mSize = {0, 0};

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
};
