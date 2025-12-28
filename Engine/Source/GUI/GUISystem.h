#pragma once
#include "Script/LuaBindable.h"

class RenderContext;

struct InputEvent;
struct ImVec4;

class GUISystem : public LuaBindable
{
public:
	GUISystem(const RenderContext& renderContext);

	void CloseGUI() const;

	void LoadGUITheme(const std::string& themeName) const;

	bool IsGUIContext() const;

	void GUIStartFrame() const;
	void GUIEndFrame() const;

	void CreatePopup(const std::string& name);
	void AddButton();

	// Debug
	void DemoWindow() const;

	void BindMethods(lua_State* state) override;
	void BindInstanceGetter(lua_State* state) override;
	static constexpr char LuaName[] = "GUISystem";
private:
	// example
	static bool show_demo_window;
};
