#pragma once
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

static bool mFirstTimeMouse = true;

class Camera
{
public:
	Camera() = default;

	glm::mat4 GetView() const;

	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();

	void UpdateView();
	void UpdateCameraSpeed();
		
private:
	glm::vec3 mCameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 mView = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);

	float mCameraSpeed = 0.05f; // adjust accordingly
	float mDeltaTime = 0.0f;	// Time between current frame and last frame
	float mLastFrame = 0.0f;    // Time of last frame
};
