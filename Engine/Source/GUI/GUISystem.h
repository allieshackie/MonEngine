#pragma once

class RenderContext;

struct InputEvent;
struct ImVec4;

class GUISystem
{
public:
	GUISystem() = default;
	~GUISystem() = default;

	static void InitGUI(const RenderContext& renderContext);
	static void CloseGUI();

	static bool IsGUIContext();

	static void RenderGuiElements();

	static void GUIStartFrame();
	static void GUIEndFrame();

private:
	// example
	static bool show_demo_window;
};
