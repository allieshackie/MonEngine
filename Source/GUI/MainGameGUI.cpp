#include "imgui/imgui.h"
#include <filesystem>
#include "Defines.h"

#include "MainGameGUI.h"

namespace fs = std::filesystem;

MainGameGUI::MainGameGUI(InputManager& inputManager, LevelManager& levelManager, MapSystem& mapSystem,
                         Renderer& renderer, ResourceManager& resourceManager)
{
	mMapEditor = std::make_unique<MapEditor>(inputManager, levelManager, mapSystem, renderer,
	                                         resourceManager);
	_GetAllLevelFileNames();
}

void MainGameGUI::RenderGUI()
{
	if (!Defines::GetInstance()->GetEditMode())
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
				Defines::GetInstance()->SetEditMode(true);
				mMapEditor->InitCameraInputs();
			}
			ImGui::EndMainMenuBar();
		}
	}
	else
	{
		mMapEditor->RenderGUI();
	}
}

void MainGameGUI::_LevelMenu(bool* p_open)
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x / 2 - (300.0f / 2), main_viewport->Size.y / 2 - (300.0f / 2)));
	ImGui::SetNextWindowSize(ImVec2(300, 300));
	if (ImGui::Begin("Load Level", p_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::Combo("Levels", &current_level_selected, levelFileNames.data(), static_cast<int>(levelFileNames.size()));
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
		const auto fileName = entry.path().filename().string();
		if (fileName.find(EDITOR_LEVEL) != std::string::npos) continue;
		levelFileNames.push_back(_strdup(fileName.c_str()));
	}
}
