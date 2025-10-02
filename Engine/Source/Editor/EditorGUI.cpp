#include "imgui.h"
#include <filesystem>

#include "Core/World.h"
#include "ContextMenus/EntityMenu.h"
#include "Input/InputHandler.h"

#include "EditorGUI.h"

EditorGUI::EditorGUI(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world, RenderContext& renderContext,
                     ResourceManager& resourceManager)
	: mInputHandler(inputHandler), mWorld(world)
{
	//mMapEditor = std::make_unique<MapEditor>(engineContext, inputHandler, levelManager, mapRegistry, renderContext);
	mObjectGUI = std::make_unique<ObjectGUI>();
	mEntityMenu = std::make_unique<EntityMenu>(inputHandler, mWorld, renderContext);
}

void EditorGUI::RenderGUI()
{
	if (ImGui::Begin("Editor", &mOpen, mWindowFlags))
	{
		if (auto world = mWorld.lock())
		{
			// Camera
			ImGui::Checkbox("Follow Cam", &world->GetCamera().GetFollowCamFlag());
			ImGui::NewLine();
		}
		else
		{
			return;
		}

		auto inputHandlerPtr = mInputHandler.lock();
		if (inputHandlerPtr == nullptr)
		{
			return;
		}

		// Inputs
		ImGui::Text("Current Key: ");
		const auto& keysPressed = inputHandlerPtr->GetKeysPressed();
		for (const auto& key : keysPressed)
		{
			ImGui::SameLine();
			ImGui::Text(inputHandlerPtr->GetNameForKey(key));
		}
		ImGui::NewLine();

		ImGui::Text("Previous Key: ");
		const auto& previousKeysPressed = inputHandlerPtr->GetPreviousKeysHeld();
		for (const auto& key : previousKeysPressed)
		{
			ImGui::SameLine();
			ImGui::Text(inputHandlerPtr->GetNameForKey(key));
		}
		ImGui::NewLine();

		mEntityMenu->Render();
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
