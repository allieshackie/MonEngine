#include "LLGL/Key.h"
#include "Camera.h"
#include "InputManager.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "RenderObjects/RenderObject.h"

#include "InteractionManager.h"

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

void InteractionManager::AddInteractableObject(const std::shared_ptr<RenderObject>& obj)
{
	mInteractableObjects.push_back(obj);
}

void InteractionManager::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	const auto ray = _CalculateMouseRay(mousePos);
	mCurrentSelectedObject = nullptr;
	for (const auto& obj : mInteractableObjects)
	{
		if (_IntersectTest(*obj, ray))
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
}

bool InteractionManager::_Intersects(const RenderObject& obj, glm::vec3 mouseRay)
{
	const auto planeNormal = _GetPlaneNormal(obj);
	const auto cameraPos = mCamera.GetPosition();

	const auto denominator = dot(mouseRay, planeNormal);

	// if denominator is 0, then ray is parallel to plane
	if (denominator > 0.001f)
	{
		const auto difference = -1 * (dot(cameraPos, planeNormal) + obj.GetPosition().z);
		if (const auto t = difference / denominator; t > 0.001f)
		{
			return true;
		}
	}

	return false;
}

glm::vec3 InteractionManager::_CalculateMouseRay(LLGL::Offset2D mousePos) const
{
	const auto normalizeCoords = mRenderer.NormalizedDeviceCoords({mousePos.x, mousePos.y, 1.0});
	const glm::vec4 homogenousClip = {normalizeCoords.x, normalizeCoords.y, -1.0f, 1.0f};
	glm::vec4 eyeRay = glm::inverse(mRenderer.GetProjection()) * homogenousClip;
	eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 1.0f);
	const glm::vec3 rayWorld = glm::inverse(mCamera.GetView()) * eyeRay;
	const auto ray = rayWorld;

	//std::cout << "Ray: " << ray.x << "," << ray.y << "," << ray.z << std::endl;

	return ray;
}

glm::vec3 InteractionManager::_GetPlaneNormal(const RenderObject& obj) const
{
	const auto& vertices = obj.GetVertices();
	const auto model = obj.GetModel();
	const glm::vec3 position = model * glm::vec4(vertices[0].position, 1.0);

	const glm::vec3 topRight = model * glm::vec4(vertices[1].position, 1.0);
	const glm::vec3 bottomLeft = model * glm::vec4(vertices[3].position, 1.0);
	const glm::vec3 widthVector = position + topRight;
	const glm::vec3 heightVector = position + bottomLeft;

	return glm::normalize(glm::cross(widthVector, heightVector));
}

bool InteractionManager::_IntersectTest(const RenderObject& obj, glm::vec3 mouseRay)
{
	const auto& vertices = obj.GetVertices();
	const auto model = obj.GetModel();

	// plane values
	const glm::vec3 position = model * glm::vec4(vertices[0].position, 1.0);

	// width/height vectors
	const glm::vec3 p1 = model * glm::vec4(vertices[1].position, 1.0);
	const glm::vec3 p2 = model * glm::vec4(vertices[2].position, 1.0);
	const glm::vec3 widthVector = p2 - position;
	const glm::vec3 heightVector = p1 - position;

	const auto normal = glm::normalize(cross(widthVector, heightVector));
	//const auto normal = cross(widthVector, heightVector);

	// ray-plane intersection check
	const auto numerator = glm::dot(position - mCamera.GetPosition(), normal);
	const auto denominator = glm::dot(mouseRay, normal);
	const auto t = numerator / denominator;

	// ray/plane intersect
	if (t >= 0)
	{
		// check if within bounds
		const auto intersectionPoint = mCamera.GetPosition() + mouseRay;
		const auto v = intersectionPoint - position;
		// TODO: Debug info
		std::cout << "Intersection: " << intersectionPoint.x << "," << intersectionPoint.y << "," << intersectionPoint.z
			<< std::endl;
		//mResourceManager.ClearDebugDrawList();
		//mResourceManager.CreateBox(mRenderer, intersectionPoint, {.1, .1, 1}, {255, 0, 0});

		const auto width = glm::length(widthVector);
		const auto height = glm::length(heightVector);

		const auto proj1 = glm::dot(v, widthVector) / width;
		const auto proj2 = glm::dot(v, heightVector) / height;

		if ((proj1 < width && proj1 > 0) && (proj2 < height && proj2 > 0))
		{
			return true;
		}
	}

	return false;
}
