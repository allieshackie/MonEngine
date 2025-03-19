#include "imgui.h"
#include <filesystem>

#include "Core/SceneManager.h"
#include "ContextMenus/EntityMenu.h"

#include "EditorGUI.h"

#include "Input/InputHandler.h"

EditorGUI::EditorGUI(const SceneManager& sceneManager, InputHandler& inputHandler)
{
	//mMapEditor = std::make_unique<MapEditor>(engineContext, inputHandler, levelManager, mapRegistry, renderContext);
	mObjectGUI = std::make_unique<ObjectGUI>();
	mEntityMenu = std::make_unique<EntityMenu>(sceneManager, inputHandler);
}

void EditorGUI::Render(MonScene* scene, ResourceManager& resourceManager, const RenderContext& renderContext,
                       const InputHandler& inputHandler)
{
	if (ImGui::Begin("Editor", &mOpen, mWindowFlags))
	{
		if (scene == nullptr)
		{
			return;
		}

		// Camera
		ImGui::Checkbox("Follow Cam", &scene->GetCamera().GetFollowCamFlag());
		ImGui::NewLine();

		// Inputs
		ImGui::Text("Current Key: ");
		const auto& keysPressed = inputHandler.GetKeysPressed();
		for (const auto& key : keysPressed)
		{
			ImGui::SameLine();
			ImGui::Text(inputHandler.GetNameForKey(key));
		}
		ImGui::NewLine();

		ImGui::Text("Previous Key: ");
		const auto& previousKeysPressed = inputHandler.GetPreviousKeysHeld();
		for (const auto& key : previousKeysPressed)
		{
			ImGui::SameLine();
			ImGui::Text(inputHandler.GetNameForKey(key));
		}
		ImGui::NewLine();

		mEntityMenu->Render(scene, renderContext);
	}
	ImGui::End();
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
