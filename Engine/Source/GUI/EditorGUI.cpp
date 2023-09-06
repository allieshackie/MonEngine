#include "imgui.h"
#include <filesystem>
#include "Core/ConfigManager.h"
#include "Core/Defines.h"
#include "Core/LevelManager.h"

#include "EditorGUI.h"

EditorGUI::EditorGUI(InputManager& inputManager, LevelManager& levelManager, MapSystem& mapSystem,
                     RenderContext& renderContext, ResourceManager& resourceManager, const ConfigManager& configManager)
{
	mMapEditor = std::make_unique<MapEditor>(inputManager, levelManager, mapSystem, renderContext,
	                                         resourceManager, configManager.GetMapsFolderPath(),
	                                         configManager.GetTexturesFolderPath());
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
