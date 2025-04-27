#include "LLGL/Key.h"

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
	if (mQueuedClick)
	{
		_OnClick(renderContext, camera, physicsSystem);
		mQueuedClick = false;
	}

	// Debug draw
	//auto ray = _CalculateMouseRay(renderContext, camera);

	//const auto& camPos = camera.GetPosition();
	//renderContext.DrawLine(camPos, camPos + glm::vec3{ray.x, ray.y, ray.z} * 100.0f, {1, 1, 1, 1});
}

void InteractSystem::_OnClick(const RenderContext& renderContext, const Camera& camera, PhysicsSystem& physicsSystem)
{
	auto ray = _CalculateMouseRay(renderContext, camera);

	const auto& camPos = camera.GetPosition();

	btVector3 rayFromWorld(camPos.x, camPos.y, camPos.z);
	btVector3 rayToWorld = rayFromWorld + btVector3{ray.x, ray.y, ray.z} * 100.0f;

	//renderContext.DrawOverlay({mMousePos.x, mMousePos.y}, {1, 0, 0, 1});

	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFromWorld, rayToWorld);
	physicsSystem.GetDynamicWorld().rayTest(rayFromWorld, rayToWorld, rayCallback);

	if (rayCallback.hasHit())
	{
		std::printf("yup!");
	}
}

InteractSystem::InteractSystem(InputHandler& inputHandler, const SceneManager& sceneManager)
{
	inputHandler.RegisterMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputHandler.RegisterButtonDownHandler(LLGL::Key::LButton, [this]() { QueueClick(); });

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

void InteractSystem::QueueClick()
{
	mQueuedClick = true;
}
