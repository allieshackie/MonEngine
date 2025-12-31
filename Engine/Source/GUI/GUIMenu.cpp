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

void GUIMenu::Close()
{
	mShouldClose = true;
}

std::shared_ptr<GUIElement> GUIMenu::AddLayout(bool horizontal)
{
	auto layout = std::make_shared<GUILayout>(horizontal ? GUILayout::Layout::Horizontal : GUILayout::Layout::Vertical);
	mElements.push_back(layout);

	return layout;
}

void GUIMenu::AddButton(std::string label, glm::vec2 offset, glm::vec2 size, LuaUtil::LuaCallback clickCallback)
{
	auto element = std::make_unique<GUIButton>(std::move(label), offset, size, std::move(clickCallback));
	mElements.push_back(std::move(element));
}

void GUIMenu::AddCombo(std::string label, std::vector<const char*> items, LuaUtil::LuaCallback onChange)
{
	auto element = std::make_unique<GUICombo>(std::move(label), std::move(items), std::move(onChange));
	mElements.push_back(std::move(element));
}

void GUIMenu::_RecalculateElementLayout()
{
}
