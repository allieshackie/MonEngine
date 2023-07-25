#include <filesystem>
#include "imgui.h"
#include "Core/LevelManager.h"
#include "Entity/Systems/PlayerSystem.h"
#include "GUI/MapEditor.h"

#include "GameGUI.h"

GameGUI::GameGUI(LevelManager& levelManager, PlayerSystem& playerSystem, std::string levelsFolderPath)
	: mLevelsFolderPath(std::move(levelsFolderPath)), mLevelManager(levelManager), mPlayerSystem(playerSystem)
{
	_GetAllLevelFileNames();
}

void GameGUI::RenderGUI()
{
	if (show_select_level_menu) _SelectLevelMenu(&show_select_level_menu);
	if (show_player_info_menu) _PlayerInfo(&show_player_info_menu);

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
			mLevelManager.LoadLevel(levelFileNames[current_level_selected]);
			mPlayerSystem.SpawnPlayer(mLevelManager.GetCurrentLevel()->GetPlayerSpawn());
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
		if (ImGui::MenuItem("Show Player Info"))
		{
			show_player_info_menu = !show_player_info_menu;
		}
		ImGui::EndMainMenuBar();
	}
}

void GameGUI::_PlayerInfo(bool* p_open) const
{
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(ImVec2(300, 300));
	if (ImGui::Begin("Player Info", p_open, ImGuiWindowFlags_MenuBar))
	{
		const auto pos = mPlayerSystem.GetPlayerPosition();
		static float position[3] = {pos.x, pos.y, pos.z};
		if (ImGui::InputFloat3("Position", position))
		{
			mPlayerSystem.SetPlayerPosition({position[0], position[1], position[2]});
		}
	}
	ImGui::End();
}

void GameGUI::_GetAllLevelFileNames()
{
	levelFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(mLevelsFolderPath))
	{
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_LEVEL) != std::string::npos) continue;
		levelFileNames.push_back(_strdup(fileName.c_str()));
	}
}
