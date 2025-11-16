#pragma once
#include "GUIMenuBase.h"

class RenderContext;

struct InputEvent;
struct ImVec4;

class GUISystem
{
public:
	GUISystem(const RenderContext& renderContext);

	void CloseGUI() const;

	void LoadGUITheme(const std::string& themeName) const;

	bool IsGUIContext() const;

	void RenderGuiElements() const;

	void GUIStartFrame() const;
	void GUIEndFrame() const;

	void RenderMenus() const;

	void AddMenu(std::unique_ptr<GUIMenuBase> element);
	void FlushMenus();

private:
	// example
	static bool show_demo_window;
	std::vector<std::unique_ptr<GUIMenuBase>> mMenus;
};
