#include "imgui.h"
#include <filesystem>

#include "Core/World.h"
#include "ContextMenus/EntityMenu.h"
#include "Input/InputHandler.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/RenderContext.h"
#include "Graphics/RenderSystem.h"

#include "EditorGUI.h"

EditorGUI::EditorGUI(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world, RenderContext& renderContext,
                     ResourceManager& resourceManager, RenderSystem& renderSystem)
	: mInputHandler(inputHandler), mWorld(world), mResourceManager(resourceManager), mRenderContext(renderContext),
	  mRenderSystem(renderSystem)
{
	//mMapEditor = std::make_unique<MapEditor>(engineContext, inputHandler, levelManager, mapRegistry, renderContext);
	mObjectGUI = std::make_unique<ObjectGUI>();
	mEntityMenu = std::make_unique<EntityMenu>(inputHandler, mWorld, renderContext);
}

void EditorGUI::RenderGUI()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Input", nullptr, &showInputSection);
			ImGui::MenuItem("Entities", nullptr, &showEntitySection);
			ImGui::MenuItem("Bones", nullptr, &showBonesSection);
			ImGui::MenuItem("Animation", nullptr, &showAnimationSection);
			ImGui::MenuItem("Bones Outline", nullptr, &showBonesOutline);
			ImGui::MenuItem("Axis", nullptr, &showAxis);
			ImGui::MenuItem("Material", nullptr, &showMaterialSection);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	ImGui::SetNextWindowSize(mSize);
	ImGui::SetNextWindowPos({mRenderContext.GetResolution().width - 250.0f, 20});
	if (ImGui::Begin("Editor", &mOpen, mWindowFlags))
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
		else
		{
			return;
		}

		if (showInputSection)
		{
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
		}

		if (showBonesSection)
		{
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
			mObjectGUI->RenderBonesGUI(mWorld, mResourceManager, mEntityMenu->GetSelectedEntity());
		}
		if (showAnimationSection)
		{
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
			mObjectGUI->RenderAnimationsGUI(mWorld, mResourceManager, mEntityMenu->GetSelectedEntity());
		}
		if (showEntitySection)
		{
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
			mEntityMenu->Render(mRenderSystem);
		}
		if (showMaterialSection)
		{
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
			mObjectGUI->RenderMaterialGUI(mRenderSystem);
		}

		RenderAxis();

		if (showBonesOutline)
		{
			mObjectGUI->RenderBonesOutline(mWorld, mResourceManager, mRenderSystem, mEntityMenu->GetSelectedEntity());
		}
	}
	ImGui::End();
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
