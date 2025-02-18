#include "imgui.h"
#include <filesystem>

#include "Core/SceneManager.h"
#include "ContextMenus/EntityMenu.h"

#include "EditorGUI.h"

EditorGUI::EditorGUI(const SceneManager& sceneManager, InputHandler& inputHandler)
{
	//mMapEditor = std::make_unique<MapEditor>(engineContext, inputHandler, levelManager, mapRegistry, renderContext);
	mObjectGUI = std::make_unique<ObjectGUI>();
	mEntityMenu = std::make_unique<EntityMenu>(sceneManager, inputHandler);
}

void EditorGUI::Render(MonScene* scene, ResourceManager& resourceManager, const RenderContext& renderContext) const
{
	mEntityMenu->Render(scene, renderContext);
	//mObjectGUI->RenderGUI(scene, resourceManager);
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
