#include "Core/Renderer.h"
#include "InputManager.h"

#include "Camera.h"

Camera::Camera(Renderer& renderer, const InputManager& inputManager)
	: mRenderer(renderer)
{
	// Register camera handlers
	inputManager.registerButtonUpHandler(LLGL::Key::Up, [=]() { MoveUp(); });
	inputManager.registerButtonUpHandler(LLGL::Key::Down, [=]() { MoveDown(); });
	inputManager.registerButtonUpHandler(LLGL::Key::Left, [=]() { MoveLeft(); });
	inputManager.registerButtonUpHandler(LLGL::Key::Right, [=]() { MoveRight(); });

	inputManager.registerZoomInHandler([=]() { ZoomIn(); });
	inputManager.registerZoomOutHandler([=]() { ZoomOut(); });

	UpdateView();
}

glm::mat4 Camera::GetView() const
{
	return mView;
}

void Camera::MoveLeft()
{
	mCameraPos.x -= mCameraSpeed;
	UpdateView();
}

void Camera::MoveRight()
{
	mCameraPos.x += mCameraSpeed;
	UpdateView();
}

void Camera::MoveUp()
{
	mCameraPos.y -= mCameraSpeed;
	UpdateView();
}

void Camera::MoveDown()
{
	mCameraPos.y += mCameraSpeed;
	UpdateView();
}

void Camera::ZoomIn()
{
	mCameraPos.z -= 0.1f;
	UpdateView();
}

void Camera::ZoomOut()
{
	mCameraPos.z += 0.1f;
	UpdateView();
}

void Camera::DebugUpdatePosition(glm::vec3 position)
{
	mCameraPos = position;
	UpdateView();
}

glm::vec3 Camera::GetFront() const
{
	return mCameraFront;
}

void Camera::SetFront(glm::vec3 front)
{
	mCameraFront = front;
	UpdateView();
}

glm::vec3 Camera::GetPosition() const
{
	return mCameraPos;
}

void Camera::SetPosition(glm::vec3 pos)
{
	mCameraPos = pos;
	UpdateView();
}

void Camera::UpdateView()
{
	mCameraFront = {mCameraPos.x, mCameraPos.y, mCameraFront.z};
	mView = glm::lookAt(mCameraPos, mCameraFront, mCameraUp);
	mRenderer.UpdateView(mView);
}
