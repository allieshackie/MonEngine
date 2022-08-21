#include "Core/RendererInstance.h"

#include "Camera.h"

glm::mat4 Camera::GetView() const
{
	return mView;
}

void Camera::MoveLeft()
{
	UpdateCameraSpeed();
	mCameraPos -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed;
	UpdateView();
}

void Camera::MoveRight()
{
	UpdateCameraSpeed();
	mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed;
	UpdateView();
}

void Camera::MoveUp()
{
	UpdateCameraSpeed();
	mCameraPos += mCameraSpeed * mCameraFront;
	UpdateView();
}

void Camera::MoveDown()
{
	UpdateCameraSpeed();
	mCameraPos -= mCameraSpeed * mCameraFront;
	UpdateView();
}

void Camera::ZoomIn()
{
	mCameraPos.z -= 0.1f;
}

void Camera::ZoomOut()
{
	mCameraPos.z += 0.1f;
}

void Camera::UpdateView()
{
	mView = glm::lookAt(mCameraPos, {0,0,0}, mCameraUp);
	RendererInstance::GetInstance()->UpdateView(mView);
}

void Camera::UpdateCameraSpeed()
{
	const float currentFrame = std::chrono::system_clock::now().time_since_epoch().count();
	mDeltaTime = currentFrame - mLastFrame;
	mLastFrame = currentFrame;
	mCameraSpeed = 2.5f * mDeltaTime;
}
