#include "imgui.h"
#include <filesystem>

#include "Core/SceneManager.h"

#include "EditorGUI.h"

EditorGUI::EditorGUI()
{
	//mMapEditor = std::make_unique<MapEditor>(engineContext, inputHandler, levelManager, mapRegistry, renderContext);
	mObjectGUI = std::make_unique<ObjectGUI>();
}

void EditorGUI::Render(MonScene* scene, ResourceManager& resourceManager) const
{
	mObjectGUI->RenderGUI(scene, resourceManager);
	// TODO: How can the camera be addressed?
	/*
	 *
	if (!Defines::GetInstance()->GetEditMode())
	{
		_MainMenu(camera);
	}
	else
	{
		mMapEditor->RenderGUI();
	}
	 */
}

void EditorGUI::_MainMenu(Camera& camera) const
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::MenuItem("Editor"))
		{
			// TODO: Set Edit mode?
			//mInputHandler.AddEditorInputs(camera);
		}
		ImGui::EndMainMenuBar();
	}
}
