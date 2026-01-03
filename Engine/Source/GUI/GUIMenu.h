#pragma once
#include <imgui.h>
#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include "Util/LuaUtil.h"

#include "GUIElement.h"

class GUIMenu
{
public:
	GUIMenu(std::string label, glm::vec2 pos, glm::vec2 size);

	void Render(const glm::vec2& viewportSize, bool viewportUpdated);
	void Close();

	bool ShouldClose() const { return mShouldClose; }

	void SetAnchor(VerticalAnchor vertical, HorizontalAnchor horizontal);
	std::shared_ptr<GUILayout> AddLayout(bool horizontal);
	void AddButton(std::string label, glm::vec2 offset, glm::vec2 size, LuaUtil::LuaCallback clickCallback);
	void AddCombo(std::string label, glm::vec2 offset, glm::vec2 size, std::vector<const char*> items, LuaUtil::LuaCallback onChange);

	const glm::vec2& GetPosition() { return mPosition; }
	const ImVec2& GetSize() { return mSize; }

	static void Bind(lua_State* state)
	{
		luaL_newmetatable(state, LuaName);

		LuaUtil::RegisterMethod<GUIMenu, &GUIMenu::AddLayout>(state, "AddLayout");
		LuaUtil::RegisterMethod<GUIMenu, &GUIMenu::AddButton>(state, "AddButton");
		LuaUtil::RegisterMethod<GUIMenu, &GUIMenu::AddCombo>(state, "AddCombo");
		LuaUtil::RegisterMethod<GUIMenu, &GUIMenu::Close>(state, "Close");
		LuaUtil::RegisterMethod<GUIMenu, &GUIMenu::SetAnchor>(state, "SetAnchor");

		lua_pushvalue(state, -1);
		lua_setfield(state, -2, "__index");

		lua_pop(state, 1);
	}

	static constexpr char LuaName[] = "GUIMenu";

private:
	std::vector<std::shared_ptr<GUIElement>> mElements;

	bool mOpen = true;
	std::string mLabel;
	glm::vec2 mPosition = { 0,0 };
	ImVec2 mSize = {0, 0};

	HorizontalAnchor mHorizontal = HorizontalAnchor::None;
	VerticalAnchor mVertical = VerticalAnchor::None;

	bool mUpdateAnchoring = false;
	bool mShouldClose = false;

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
};
