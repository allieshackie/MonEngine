#pragma once
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

class InputManager;
class Renderer;

class Camera
{
public:
	Camera(Renderer& renderer, const InputManager& inputManager);
	~Camera() = default;

	glm::mat4 GetView() const;

	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();

	void ZoomIn();
	void ZoomOut();

	void DebugUpdatePosition(glm::vec3 position)
	{
		mCameraPos = position;
		UpdateView();
	}

	glm::vec3 GetPosition() const { return mCameraPos; }
	glm::vec3 GetFront() const { return mCameraFront; }

	void SetPosition(glm::vec3 pos)
	{
		mCameraPos = pos;
		UpdateView();
	}

	void SetFront(glm::vec3 front)
	{
		mCameraFront = front;
		UpdateView();
	}

	void UpdateView();

private:
	glm::vec3 mCameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 mCameraUp = glm::vec3(0.0f, -1.0f, 0.0f);

	glm::mat4 mView = lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);

	float mCameraSpeed = 0.1f; // adjust accordingly

	Renderer& mRenderer;
};
