#include <filesystem>
#include "imgui.h"
#include "Core/EngineContext.h"

#include "GameGUI.h"

GameGUI::GameGUI(EngineContext& engineContext)
	: mEngineContext(engineContext)
{
	_GetAllLevelFileNames();
}

void GameGUI::RenderGUI()
{
	if (show_select_level_menu) _SelectLevelMenu(&show_select_level_menu);

	if (show_main_menu)
	{
		_MainMenu();
	}
	else
	{
		_LevelMenu();
	}
}

void GameGUI::_MainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::MenuItem("Play"))
		{
			show_select_level_menu = !show_select_level_menu;
		}
		ImGui::EndMainMenuBar();
	}
}

void GameGUI::_SelectLevelMenu(bool* p_open)
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x / 2 - (300.0f / 2), main_viewport->Size.y / 2 - (300.0f / 2)));
	ImGui::SetNextWindowSize(ImVec2(300, 300));
	if (ImGui::Begin("Load Level", p_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::Combo("Levels", &current_level_selected, levelFileNames.data(), static_cast<int>(levelFileNames.size()));
		if (ImGui::Button("Open"))
		{
			mEngineContext.LoadLevel(levelFileNames[current_level_selected]);
			show_select_level_menu = false;
			show_main_menu = false;
		}
	}
	ImGui::End();
}

void GameGUI::_LevelMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		/*
		 *
		if (ImGui::MenuItem("Show Player Info"))
		{
			show_player_info_menu = !show_player_info_menu;
		}
		 */
		ImGui::EndMainMenuBar();
	}
}

void GameGUI::_GetAllLevelFileNames()
{
	levelFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(LEVELS_FOLDER))
	{
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_LEVEL) != std::string::npos) continue;
		levelFileNames.push_back(_strdup(fileName.c_str()));
	}
}
