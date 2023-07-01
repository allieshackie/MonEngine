#include "imgui.h"
#include <filesystem>
#include "Core/Defines.h"
#include "Core/LevelManager.h"

#include "EditorGUI.h"

EditorGUI::EditorGUI(InputManager& inputManager, LevelManager& levelManager, MapSystem& mapSystem,
                     Renderer& renderer, ResourceManager& resourceManager)
{
	mMapEditor = std::make_unique<MapEditor>(inputManager, levelManager, mapSystem, renderer,
	                                         resourceManager);
}

void EditorGUI::RenderGUI()
{
	if (!Defines::GetInstance()->GetEditMode())
	{
		_MainMenu();
	}
	else
	{
		mMapEditor->RenderGUI();
	}
}

void EditorGUI::_MainMenu() const
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::MenuItem("Editor"))
		{
			Defines::GetInstance()->SetEditMode(true);
			mMapEditor->InitCameraInputs();
		}
		ImGui::EndMainMenuBar();
	}
}
