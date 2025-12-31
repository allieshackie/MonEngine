#pragma once
#include <imgui.h>
#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include "Util/LuaUtil.h"

#include "GUIElement.h"

enum class HorizontalAnchor
{
	None,
	Left,
	Center,
	Right
};

enum class VerticalAnchor
{
	None,
	Top,
	Center,
	Bottom,
};

class GUIMenu
{
public:
	GUIMenu(std::string label, glm::vec2 pos, glm::vec2 size);

	void Render();
	void Close();

	bool ShouldClose() const { return mShouldClose; }

	std::shared_ptr<GUIElement> AddLayout(bool horizontal);
	void AddButton(std::string label, glm::vec2 offset, glm::vec2 size, LuaUtil::LuaCallback clickCallback);
	void AddCombo(std::string label, std::vector<const char*> items, LuaUtil::LuaCallback onChange);

	static void Bind(lua_State* state)
	{
		luaL_newmetatable(state, LuaName);

		LuaUtil::RegisterMethod<GUIMenu, &GUIMenu::AddButton>(state, "AddButton");
		LuaUtil::RegisterMethod<GUIMenu, &GUIMenu::AddCombo>(state, "AddCombo");
		LuaUtil::RegisterMethod<GUIMenu, &GUIMenu::Close>(state, "Close");

		lua_pushvalue(state, -1);
		lua_setfield(state, -2, "__index");

		lua_pop(state, 1);
	}

	static constexpr char LuaName[] = "GUIMenu";

private:
	void _RecalculateElementLayout();

	std::vector<std::shared_ptr<GUIElement>> mElements;

	bool mOpen = true;
	std::string mLabel;
	ImVec2 mPosition = {0, 0};
	ImVec2 mSize = {0, 0};

	HorizontalAnchor mHorizontal = HorizontalAnchor::None;
	VerticalAnchor mVertical = VerticalAnchor::None;

	bool mShouldClose = false;

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
};
