#pragma once
#include <LLGL/LLGL.h>
#include "imgui/imgui.h"
#include "LLGL/Platform/Win32/Win32NativeHandle.h"

struct InputEvent;

class GUISystem
{
public:
	GUISystem(const Renderer& renderer);
	~GUISystem();

	void initGUI(const Renderer& renderer);
	void closeGUI() const;

	void handleGUIInput(const InputEvent& inputEvent);
	bool isGUIContext();

	void RenderGuiElements();

	void GUIStartFrame();
	void GUIEndFrame();

private:
	void _handleButtonDown(LLGL::Key keycode) const;
	void _handleButtonUp(LLGL::Key keycode) const;

	const char* GLSL_VERSION = "#version 130";
	HWND mNativeWindow = nullptr;

	// example
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool show_demo_window = true;
};
