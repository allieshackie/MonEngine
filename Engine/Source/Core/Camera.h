#pragma once
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);

	glm::mat4 GetView() const;

	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();

	void ZoomIn();
	void ZoomOut();

	void DebugUpdatePosition(glm::vec3 position);

	glm::vec3 GetPosition() const;
	glm::vec3 GetFront() const;

	void SetPosition(const glm::vec3 pos);
	void SetFront(const glm::vec3 front);
	void SetUp(const glm::vec3 up);

	void UpdateView();

private:
	glm::vec3 mCameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 mView = glm::identity<glm::mat4>();

	float mCameraSpeed = 0.1f; // adjust accordingly
};
