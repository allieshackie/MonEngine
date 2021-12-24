#pragma once
#include <SDL_video.h>
#include "imgui.h"

class SDL_Window;

class GUISystem
{
public:
	GUISystem(SDL_Window& windowRef, SDL_GLContext contextRef);
	~GUISystem();
	
	void initGUI() const;
	void renderGUI();
	void closeGUI() const;

private:
	const char* GLSL_VERSION = "#version 130";

	SDL_Window& mWindow;
	SDL_GLContext mContext;

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};