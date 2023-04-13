#include <filesystem>
#include "Defines.h"

#include "MainGameGUI.h"

namespace fs = std::filesystem;

MainGameGUI::MainGameGUI()
{
	_GetAllLevelFileNames();
}

void MainGameGUI::RenderGUI()
{
	if (show_level_menu) _LevelMenu(&show_level_menu);

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::MenuItem("Play"))
		{
			show_level_menu = !show_level_menu;
		}
		if (ImGui::MenuItem("Editor"))
		{
			MonDev::EDIT_MODE = true;
		}
		ImGui::EndMainMenuBar();
	}
}

void MainGameGUI::_LevelMenu(bool* p_open)
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x / 2 - (300 / 2), main_viewport->Size.y / 2 - (300 / 2)));
	ImGui::SetNextWindowSize(ImVec2(300, 300));
	if (ImGui::Begin("Load Level", p_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::Combo("Levels", &current_level_selected, levelFileNames.data(), levelFileNames.size());
		if (ImGui::Button("Open"))
		{
		}
	}
	ImGui::End();
}

void MainGameGUI::_GetAllLevelFileNames()
{
	levelFileNames.clear();
	for (const auto& entry : fs::directory_iterator(LEVELS_FOLDER))
	{
		levelFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}
}
