#include "GUIMenu.h"

GUIMenu::GUIMenu(std::string label, glm::vec2 pos, glm::vec2 size)
	: mLabel(std::move(label)), mPosition(ImVec2(pos.x, pos.y)), mSize(ImVec2(size.x, size.y))
{
}

void GUIMenu::Render()
{
	ImGui::SetNextWindowSize(mSize);
	if (ImGui::Begin(mLabel.c_str(), &mOpen, mWindowFlags))
	{
		for (const auto& element : mElements)
		{
			element->Render();
		}
	}
	ImGui::End();
}

void GUIMenu::AddElement(std::unique_ptr<GUIElement> element)
{
	mElements.push_back(std::move(element));
}
