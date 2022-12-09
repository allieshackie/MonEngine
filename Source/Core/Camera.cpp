#include "Core/Renderer.h"

#include "Camera.h"

Camera::Camera(Renderer& renderer)
	: mRenderer(renderer)
{
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

void Camera::UpdateView()
{
	mView = lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
	mRenderer.UpdateView(mView);
}
