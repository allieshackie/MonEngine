#pragma once
#include <glm/vec2.hpp>
#include "Script/LuaBindable.h"

class GUIMenu;
class WindowContext;

struct InputEvent;
struct ImVec4;

class GUISystem : public LuaBindable
{
public:
	GUISystem(WindowContext& windowContext);

	void RenderMenus();

	void CloseGUI() const;

	void LoadGUITheme(const std::string& themeName) const;

	bool IsGUIContext() const;

	void GUIStartFrame() const;
	void GUIEndFrame() const;

	std::shared_ptr<GUIMenu> CreatePopup(const std::string& name, glm::vec2 pos, glm::vec2 size);

	// Debug
	void DemoWindow() const;

	void BindMethods(lua_State* state) override;
	void BindInstanceGetter(lua_State* state) override;
	static constexpr char LuaName[] = "GUISystem";
private:
	// example
	static bool show_demo_window;
	std::vector<std::shared_ptr<GUIMenu>> mGUIMenus;
	std::vector<std::shared_ptr<GUIMenu>> mPendingMenus;
	WindowContext& mWindowContext;

	glm::vec2 mViewportSize = { 0,0 };
};
