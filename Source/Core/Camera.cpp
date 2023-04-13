#include "Core/Renderer.h"
#include "InputManager.h"
#include "Defines.h"

#include "Camera.h"

Camera::Camera(Renderer& renderer, const InputManager& inputManager)
	: mRenderer(renderer)
{
	// Register camera handlers for moving the camera position
	// If the mCameraFront remains the same, this will result in the
	// camera view angling.  We can adjust the mCameraFront.xy to match the
	// camera position so that the view will not angle
	if (MonDev::EDIT_MODE)
	{
		inputManager.registerButtonUpHandler(LLGL::Key::Up, [=]() { MoveUp(); });
		inputManager.registerButtonUpHandler(LLGL::Key::Down, [=]() { MoveDown(); });
		inputManager.registerButtonUpHandler(LLGL::Key::Left, [=]() { MoveLeft(); });
		inputManager.registerButtonUpHandler(LLGL::Key::Right, [=]() { MoveRight(); });

		// Handlers for handling the camera zoom
		inputManager.registerZoomInHandler([=]() { ZoomIn(); });
		inputManager.registerZoomOutHandler([=]() { ZoomOut(); });
	}

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

/*
 * The view matrix is used in the projection-view-model matrix calculation to
 * determine a vertex's final position in clip space
 * The view matrix is queried by the Renderer/Pipelines to update the mvp matrix
 * uniform in the shader files
 */
void Camera::UpdateView()
{
	mView = glm::lookAt(mCameraPos, mCameraFront, mCameraUp);
	mRenderer.UpdateView(mView);
}
