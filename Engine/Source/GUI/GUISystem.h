#pragma once

#include "LLGL/Platform/Win32/Win32NativeHandle.h"

class Renderer;

struct InputEvent;
struct ImVec4;

class GUISystem
{
public:
	GUISystem(const Renderer& renderer);
	~GUISystem();

	void initGUI(const Renderer& renderer);
	void closeGUI() const;

	bool isGUIContext();

	void RenderGuiElements();

	void GUIStartFrame();
	void GUIEndFrame();

private:
	const char* GLSL_VERSION = "#version 460";
	HWND mNativeWindow = nullptr;

	// example
	bool show_demo_window = true;
};
