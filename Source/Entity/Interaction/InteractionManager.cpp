#include "LLGL/Key.h"
#include "Camera.h"
#include "InputManager.h"
#include "Core/Renderer.h"
#include "RenderObjects/RenderObject.h"

#include "InteractionManager.h"

#include "Core/ResourceManager.h"

InteractionManager::InteractionManager(Renderer& renderer, Camera& camera, const InputManager& inputManager,
                                       ResourceManager& resourceManager)
	: mCamera(camera), mRenderer(renderer), mResourceManager(resourceManager)
{
	inputManager.registerMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputManager.registerButtonDownHandler(LLGL::Key::LButton, [this]() { _OnClick(); });
}

void InteractionManager::RegisterAction(std::function<void(RenderObject&)> cb)
{
	mActionCallbacks.push_back(std::move(cb));
}

void InteractionManager::AddInteractableObject(RenderObject* obj)
{
	mInteractableObjects.push_back(obj);
}

void InteractionManager::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	const auto ray = _CalculateMouseRay(mousePos);
	for (const auto obj : mInteractableObjects)
	{
		if (_Intersects(*obj, ray))
		{
			mCurrentSelectedObject = obj;
		}
	}
}

void InteractionManager::_OnClick() const
{
	if (mCurrentSelectedObject)
	{
		for (const auto& cb : mActionCallbacks)
		{
			cb(*mCurrentSelectedObject);
		}
	}

	mResourceManager.ClearDebugDrawList();
	mResourceManager.CreateBox(mRenderer, lastMousePos, {1, 1, 1}, {255, 0, 0});
}

bool InteractionManager::_Intersects(RenderObject& obj, glm::vec3 mouseRay)
{
	const auto planeNormal = _GetPlaneNormal(&obj);
	const auto cameraPos = mCamera.GetPosition();

	const auto denominator = dot(mouseRay, planeNormal);

	// if denominator is 0, then ray is parallel to plane
	if (denominator > 0.001f)
	{
		const auto difference = -1 * (dot(cameraPos, planeNormal) + obj.GetPosition().z);
		if (const auto t = difference / denominator; t > 0.001f)
		{
			lastMousePos = {
				cameraPos.x + mouseRay.x * t, cameraPos.y + mouseRay.y * t, cameraPos.z + mouseRay.z * t
			};
			return true;
		}
	}

	return false;
}

glm::vec3 InteractionManager::_CalculateMouseRay(LLGL::Offset2D mousePos)
{
	const auto normalizeCoords = mRenderer.NormalizedDeviceCoords({mousePos.x, mousePos.y, 1.0});
	const glm::vec4 homogenousClip = {normalizeCoords.x, normalizeCoords.y, -1.0f, 1.0f};
	glm::vec4 eyeRay = inverse(mRenderer.GetProjection()) * homogenousClip;
	eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 1.0f);
	const glm::vec3 rayWorld = inverse(mCamera.GetView()) * eyeRay;
	const glm::vec3 mousePoint = normalize(rayWorld);

	return mousePoint;
}

glm::vec3 InteractionManager::_GetPlaneNormal(const RenderObject* obj) const
{
	const auto& vertices = obj->GetVertices();
	const auto model = obj->GetModel();

	const glm::vec3 topLeft = model * glm::vec4(vertices[0].position, 1.0);
	const glm::vec3 bottomLeft = model * glm::vec4(vertices[1].position, 1.0);
	const glm::vec3 bottomRight = model * glm::vec4(vertices[2].position, 1.0);

	const auto qr = bottomRight - bottomLeft;
	const auto qs = topLeft - bottomLeft;

	const auto crossVal = cross(qr, qs);
	return crossVal / length(crossVal);
}
