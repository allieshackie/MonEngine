#include "imgui.h"
#include <filesystem>

#include "Core/LevelManager.h"
#include "Input/InputHandler.h"

#include "EditorGUI.h"

EditorGUI::EditorGUI(EngineContext& engineContext, InputHandler& inputHandler, LevelManager& levelManager,
                     MapRegistry& mapRegistry, RenderContext& renderContext) : mInputHandler(inputHandler)
{
	mMapEditor = std::make_unique<MapEditor>(engineContext, inputHandler, levelManager, mapRegistry, renderContext);
}

void EditorGUI::RenderGUI()
{
	// TODO: How can the camera be addressed?
	/*
	 *
	if (!Defines::GetInstance()->GetEditMode())
	{
		_MainMenu();
	}
	else
	{
		mMapEditor->RenderGUI();
	}
	 */
}

void EditorGUI::_MainMenu(const std::unique_ptr<Camera>& camera) const
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::MenuItem("Editor"))
		{
			// TODO: Set Edit mode?
			mInputHandler.AddEditorInputs(*camera);
		}
		ImGui::EndMainMenuBar();
	}
}
