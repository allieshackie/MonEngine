#include <imgui.h>
#include "LLGL/Key.h"
#include "LLGL/Platform/Win32/Win32NativeHandle.h"

#include "Core/EventListener.h"
#include "Core/SceneManager.h"
#include "Entity/Entity.h"
#include "Entity/Components/InteractComponent.h"
#include "Input/InputHandler.h"
#include "Graphics/RenderContext.h"
#include "Physics/PhysicsSystem.h"

#include "InteractSystem.h"

void InteractSystem::Update(const RenderContext& renderContext, const Camera& camera, PhysicsSystem& physicsSystem)
{
	if (mQueueIntersectCheck)
	{
		_OnClick(renderContext, camera, physicsSystem);
		mQueueIntersectCheck = false;
	}
}

void InteractSystem::Render()
{
	if (mShowOptions)
	{
		_ShowOptions();
	}
}

void InteractSystem::_OnClick(const RenderContext& renderContext, const Camera& camera,
                              PhysicsSystem& physicsSystem) const
{
	auto ray = _CalculateMouseRay(renderContext, camera);

	const auto& camPos = camera.GetPosition();

	btVector3 rayFromWorld(camPos.x, camPos.y, camPos.z);
	btVector3 rayToWorld = rayFromWorld + btVector3{ray.x, ray.y, ray.z} * 100.0f;

	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFromWorld, rayToWorld);
	physicsSystem.GetDynamicWorld().rayTest(rayFromWorld, rayToWorld, rayCallback);

	if (rayCallback.hasHit())
	{
		std::printf("yup!");
		ReleaseCapture();
		LLGL::NativeHandle mainWindowHandle;
		renderContext.GetSurfaceNativeHandle(&mainWindowHandle, sizeof(mainWindowHandle));
		SendMessage(mainWindowHandle.window, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}

InteractSystem::InteractSystem(InputHandler& inputHandler, const SceneManager& sceneManager)
{
	inputHandler.RegisterMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputHandler.RegisterButtonDownHandler(LLGL::Key::LButton, [this]() { DragClick(); });

	inputHandler.RegisterButtonDownHandler(LLGL::Key::RButton, [this]() { mShowOptions = !mShowOptions; });
	//inputHandler.RegisterButtonUpHandler(LLGL::Key::RButton, [this]() { mShowOptions = false; });

	// All entities should have a transform
	EventFunc addedFunc = [this](Entity* entity)
	{
		OnEntityAdded(entity);
	};
	sceneManager.ConnectOnConstruct<InteractComponent>(addedFunc);
	EventFunc destroyFunc = [this](Entity* entity)
	{
		OnEntityRemoved(entity);
	};
	sceneManager.ConnectOnDestroy<InteractComponent>(destroyFunc);
}

void InteractSystem::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	mMousePos = {mousePos.x, mousePos.y};
}

glm::vec3 InteractSystem::_CalculateMouseRay(const RenderContext& renderContext, const Camera& camera) const
{
	const auto normalizeCoords = renderContext.NormalizedDeviceCoords({mMousePos.x, mMousePos.y, 1.0});
	const glm::vec4 homogenousClip = {normalizeCoords.x, normalizeCoords.y, -1.0f, 1.0f};
	glm::vec4 eyeRay = glm::inverse(renderContext.GetProjection()) * homogenousClip;
	eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 0.0f);
	return glm::normalize(glm::inverse(camera.GetView()) * eyeRay);
}

void InteractSystem::_ShowOptions()
{
	ImGui::SetNextWindowSize(ImVec2(150, 100));
	if (ImGui::Begin("Doge", &mShowOptions, ImGuiWindowFlags_None))
	{
		ImGui::Button("Chat with Doge", {120, 20});
	}
	ImGui::End();
}

void InteractSystem::OnEntityAdded(Entity* entity)
{
	mEntityList.push_back(entity->GetId());
}

void InteractSystem::OnEntityRemoved(Entity* entity)
{
	const auto it = std::find(mEntityList.begin(), mEntityList.end(), entity->GetId());
	if (it != mEntityList.end())
	{
		mEntityList.erase(it);
	}
}

void InteractSystem::DragClick()
{
	mQueueIntersectCheck = true;
}
