#include "imgui.h"
#include <filesystem>

#include "Core/SceneManager.h"
#include "Core/World.h"
#include "ContextMenus/EntityMenu.h"
#include "Input/InputHandler.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/WindowContext.h"
#include "Graphics/RenderSystem.h"

#include "EditorGUI.h"

EditorGUI::EditorGUI(std::weak_ptr<InputHandler> inputHandler, RenderSystem& renderSystem,
					 ResourceManager& resourceManager, SceneManager& sceneManager,
					 WindowContext& windowContext, std::weak_ptr<World> world)
	: mInputHandler(inputHandler), mRenderSystem(renderSystem), mResourceManager(resourceManager),
	   mSceneManager(sceneManager), mWindowContext(windowContext), mWorld(world)
{
	//mMapEditor = std::make_unique<MapEditor>(engineContext, inputHandler, levelManager, mapRegistry, WindowContext);
	mObjectGUI = std::make_unique<ObjectGUI>();
	mEntityMenu = std::make_unique<EntityMenu>(inputHandler, mWorld, renderSystem);
}

void EditorGUI::RenderGUI(float dt, float fps)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Level"))
		{
			if (ImGui::MenuItem("Restart"))
			{
				mSceneManager.RestartScene();
			}
			if (ImGui::MenuItem("Save"))
			{

			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Editor"))
		{
			ImGui::MenuItem("FPS", nullptr, &showFPS);
			ImGui::MenuItem("Axis", nullptr, &showAxis);
			ImGui::MenuItem("Bones Outline", nullptr, &showBonesOutline);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Camera", nullptr, &showCameraWindow);
			ImGui::MenuItem("Input", nullptr, &showInputWindow);
			ImGui::MenuItem("Properties", nullptr, &showEntityWindow);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	_RenderCameraWindow();
	_RenderInputWindow();
	_RenderPropertiesWindow();
	_RenderFPSWindow(fps);

	RenderAxis();

	if (showBonesOutline)
	{
		mObjectGUI->RenderBonesOutline(mWorld, mResourceManager, mRenderSystem, mEntityMenu->GetSelectedEntity());
	}
}

void EditorGUI::RenderAxis()
{
	if (auto world = mWorld.lock())
	{
		if (showAxis)
		{
			if (mAxisElement == -1)
			{
				const std::vector<DebugVertex> vertices = {
					{{0, 0, 0}, {1, 0, 0, 1}}, {{1, 0, 0}, {1, 0, 0, 1}}, // X axis - red
					{{0, 0, 0}, {0, 1, 0, 1}}, {{0, 1, 0}, {0, 1, 0, 1}}, // Y axis - green
					{{0, 0, 0}, {0, 0, 1, 1}}, {{0, 0, 1}, {0, 0, 1, 1}} // Z axis - blue
				};

				auto axisRotation = glm::mat4(glm::mat3(world->GetCamera().GetView()));
				axisRotation = glm::inverse(axisRotation);
				glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(50));
				model = axisRotation * model;
				model = glm::translate(glm::mat4(1.0f), glm::vec3(60, 50, -1)) * model;

				mAxisElement = mRenderSystem.AddOverlay(vertices, model);
			}
			auto axisRotation = glm::mat4(glm::mat3(world->GetCamera().GetView()));
			axisRotation = glm::inverse(axisRotation);
			glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(50));
			model = axisRotation * model;
			model = glm::translate(glm::mat4(1.0f), glm::vec3(60, 50, -1)) * model;

			mRenderSystem.UpdateOverlayTransform(mAxisElement, model);
		}
		else if (mAxisElement != -1 && !showAxis)
		{
			mRenderSystem.ClearOverlay();
			mAxisElement = -1;
		}
	}
}

void EditorGUI::_RenderCameraWindow()
{
	if (!showCameraWindow)
	{
		return;
	}
	if (ImGui::Begin("Camera", &showCameraWindow, mWindowFlags))
	{
		if (auto world = mWorld.lock())
		{
			// Camera
			ImGui::Checkbox("Follow Cam", &world->GetCamera().GetFollowCamFlag());
			ImGui::NewLine();

			// Camera Follow Offset
			ImGui::Text("Camera Follow Offset: ");
			ImGui::DragFloat("FollowX", &world->GetCamera().GetCameraFollowOffsetX(), 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("FollowY", &world->GetCamera().GetCameraFollowOffsetY(), 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("FollowZ", &world->GetCamera().GetCameraFollowOffsetZ(), 0.1f, -100.0f, 100.0f);
			ImGui::NewLine();

			// Camera Look Offset
			ImGui::Text("Camera Look Offset: ");
			ImGui::DragFloat("LookX", &world->GetCamera().GetCameraLookOffsetX(), 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("LookY", &world->GetCamera().GetCameraLookOffsetY(), 0.1f, -100.0f, 100.0f);
			ImGui::DragFloat("LookZ", &world->GetCamera().GetCameraLookOffsetZ(), 0.1f, -100.0f, 100.0f);
			ImGui::NewLine();
		}
	}

	ImGui::End();
}

void EditorGUI::_RenderInputWindow()
{
	if (!showInputWindow)
	{
		return;
	}
	auto inputHandlerPtr = mInputHandler.lock();
	if (inputHandlerPtr == nullptr)
	{
		return;
	}

	if (ImGui::Begin("Input", &showInputWindow, mWindowFlags))
	{
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
	}
	ImGui::End();
}

void EditorGUI::_RenderPropertiesWindow()
{
	if (!showEntityWindow)
	{
		return;
	}

	if (ImGui::Begin("Properties", &showEntityWindow, mWindowFlags))
	{
		mEntityMenu->Render(mRenderSystem);

		ImGui::SeparatorText("Animation");
		mObjectGUI->RenderAnimationsGUI(mWorld, mResourceManager, mEntityMenu->GetSelectedEntity());
		mObjectGUI->RenderBonesGUI(mWorld, mResourceManager, mEntityMenu->GetSelectedEntity());
	
		ImGui::SeparatorText("Material");
		mObjectGUI->RenderMaterialGUI(mRenderSystem);
	}

	ImGui::End();
}

void EditorGUI::_RenderFPSWindow(float fps)
{
	if (!showFPS)
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always,ImVec2(0.0f, 1.0f));

	ImGui::SetNextWindowBgAlpha(0.0f);      
	if (ImGui::Begin("Fps", nullptr, mHiddenWindowFlags))
	{
		ImGui::Text("FPS: %.1f", fps);
	}
	ImGui::End();
}
