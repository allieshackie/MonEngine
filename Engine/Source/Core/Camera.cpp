#include "Entity/Components/PlayerComponent.h"
#include <Entity/Components/TransformComponent.h>

#include "Camera.h"

Camera::Camera(EntityRegistry& entityRegistry, glm::vec3 position, glm::vec3 front, glm::vec3 up)
	: mCameraPos(position), mCameraFront(front), mCameraUp(up)
{
	UpdateView();

	entityRegistry.GetEnttRegistry().on_construct<PlayerComponent>().connect<&Camera::SetLookTarget>(this);
}

void Camera::Update(EntityRegistry& entityRegistry)
{
	if (mFollowCam)
	{
		const auto& transform = entityRegistry.GetComponent<TransformComponent>(mCameraTargetEntity);
		if (transform.mPosition != mCameraPos)
		{
			mCameraPos = transform.mPosition;
			mCameraPos += mCameraFollowOffset;
			mCameraTarget = transform.mPosition;
			mCameraTarget += mCameraLookOffset;
			UpdateView();
		}
	}
}

void Camera::MoveLeft()
{
	if (mFollowCam) return;
	mCameraPos.x -= mCameraSpeed;
	UpdateView();
}

void Camera::MoveRight()
{
	if (mFollowCam) return;
	mCameraPos.x += mCameraSpeed;
	UpdateView();
}

void Camera::MoveUp()
{
	if (mFollowCam) return;
	mCameraPos.y += mCameraSpeed;
	UpdateView();
}

void Camera::MoveDown()
{
	if (mFollowCam) return;
	mCameraPos.y -= mCameraSpeed;
	UpdateView();
}

void Camera::ZoomIn()
{
	if (mFollowCam) return;
	mCameraPos.z += 0.5f;
	UpdateView();
}

void Camera::ZoomOut()
{
	if (mFollowCam) return;
	mCameraPos.z -= 0.5f;
	UpdateView();
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

void Camera::UpdateRight()
{
	if (glm::abs(glm::dot(mCameraFront, mCameraUp)) > 0.9999f)
	{
		// mCameraFront and mCameraUp are nearly parallel; set mCameraRight to a fixed perpendicular vector
		mCameraRight = glm::vec3(1.0f, 0.0f, 0.0f); // or (0.0f, 0.0f, 1.0f) if more suitable for your system
	}
	else
	{
		// Calculate mCameraRight normally
		mCameraRight = glm::normalize(glm::cross(mCameraFront, mCameraUp));
	}
}

void Camera::SetLookTarget(EnTTRegistry& registry, EntityId entity)
{
	mCameraTargetEntity = entity;
	mFollowCam = true;
}

/*
 * The view matrix is used in the projection-view-model matrix calculation to
 * determine a vertex's final position in clip space
 * The view matrix is queried by the Renderer/Pipelines to update the mvp matrix
 * uniform in the shader files
 */
void Camera::UpdateView()
{
	mView = glm::lookAt(mCameraPos, mCameraTarget, mCameraUp);
	UpdateRight();
}
