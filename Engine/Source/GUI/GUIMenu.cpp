#include "GUIMenu.h"

GUIMenu::GUIMenu(std::string label, glm::vec2 pos, glm::vec2 size)
	: mLabel(std::move(label)), mPosition(pos), mSize(ImVec2(size.x, size.y))
{
}

void GUIMenu::Render(const glm::vec2& viewportSize, bool viewportUpdated)
{
	if (mUpdateAnchoring || viewportUpdated)
	{
		switch (mHorizontal)
		{
		case HorizontalAnchor::Left: mPosition.x = 0; break;
		case HorizontalAnchor::Center: mPosition.x = (viewportSize.x * 0.5f) - (mSize.x * 0.5f); break;
		case HorizontalAnchor::Right: mPosition.x = viewportSize.x - mSize.x; break;
		}
		switch (mVertical)
		{
		case VerticalAnchor::Top: mPosition.y = 0; break;
		case VerticalAnchor::Center: mPosition.y = viewportSize.y * 0.5f - (mSize.y * 0.5f); break;
		case VerticalAnchor::Bottom: mPosition.y = viewportSize.y - mSize.y; break;
		}

		mUpdateAnchoring = false;
	}

	ImGui::SetNextWindowPos({ mPosition.x, mPosition.y });
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

void GUIMenu::SetAnchor(VerticalAnchor vertical, HorizontalAnchor horizontal)
{
	mVertical = vertical;
	mHorizontal = horizontal;

	mUpdateAnchoring = true;
}

std::shared_ptr<GUILayout> GUIMenu::AddLayout(bool horizontal)
{
	auto layout = std::make_shared<GUILayout>(horizontal ? GUIElement::Layout::Horizontal : GUIElement::Layout::Vertical, glm::vec2{mSize.x * 0.5f, mSize.y * 0.5f});
	mElements.push_back(layout);

	return layout;
}

void GUIMenu::AddButton(std::string label, glm::vec2 offset, glm::vec2 size, LuaUtil::LuaCallback clickCallback)
{
	auto element = std::make_shared<GUIButton>(std::move(label), offset, size, std::move(clickCallback));

	// Adding a button outside a layout means the "offset" is just the pure position
	element->SetPosition(offset);

	mElements.push_back(std::move(element));
}

void GUIMenu::AddCombo(std::string label, glm::vec2 offset, glm::vec2 size, std::vector<const char*> items, LuaUtil::LuaCallback onChange)
{
	auto element = std::make_shared<GUICombo>(std::move(label), offset, size, std::move(items), std::move(onChange));

	// Adding a combo outside a layout means the "offset" is just the pure position
	element->SetPosition(offset);

	mElements.push_back(std::move(element));
}
