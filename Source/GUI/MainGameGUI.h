#pragma once
#include "imgui/imgui.h"

class MainGameGUI
{
public:
	MainGameGUI();
	~MainGameGUI() = default;

	void RenderGUI();

private:
	void _LevelMenu(bool* p_open);
	void _GetAllLevelFileNames();

	bool show_level_menu = false;

	int current_level_selected = 0;

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	std::vector<char*> levelFileNames;

	const char* LEVELS_FOLDER = "../Data/Levels/";
};
