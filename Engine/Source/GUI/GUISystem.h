#pragma once
#include "GUIMenuBase.h"

class RenderContext;

struct InputEvent;
struct ImVec4;

class GUISystem
{
public:
	static void InitGUI(const RenderContext& renderContext);
	static void CloseGUI();

	static bool IsGUIContext();

	static void RenderGuiElements();

	static void GUIStartFrame();
	static void GUIEndFrame();

	static void RenderMenus();

	static void AddMenu(std::unique_ptr<GUIMenuBase> element);
	static void FlushMenus();

private:
	// example
	static bool show_demo_window;
	static std::vector<std::unique_ptr<GUIMenuBase>> mMenus;
};
