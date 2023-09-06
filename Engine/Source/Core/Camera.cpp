#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
	: mCameraPos(position), mCameraFront(front), mCameraUp(up)
{
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
	mCameraPos.y += mCameraSpeed;
	UpdateView();
}

void Camera::MoveDown()
{
	mCameraPos.y -= mCameraSpeed;
	UpdateView();
}

void Camera::ZoomIn()
{
	mCameraPos.z += 0.3f;
	UpdateView();
}

void Camera::ZoomOut()
{
	mCameraPos.z -= 0.3f;
	UpdateView();
}

void Camera::DebugUpdatePosition(glm::vec3 position)
{
	mCameraPos = position;
	UpdateView();
}

glm::vec3 Camera::GetPosition() const
{
	return mCameraPos;
}

glm::vec3 Camera::GetFront() const
{
	return mCameraFront;
}

void Camera::SetPosition(const glm::vec3 pos)
{
	mCameraPos = pos;
	UpdateView();
}

void Camera::SetFront(const glm::vec3 front)
{
	mCameraFront = front;
	UpdateView();
}

void Camera::SetUp(const glm::vec3 up)
{
	mCameraUp = up;
	UpdateView();
}

/*
 * The view matrix is used in the projection-view-model matrix calculation to
 * determine a vertex's final position in clip space
 * The view matrix is queried by the Renderer/Pipelines to update the mvp matrix
 * uniform in the shader files
 */
void Camera::UpdateView()
{
	mView = glm::lookAt(mCameraPos, mCameraFront, mCameraUp);
}
