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

	void DebugUpdatePosition(glm::vec3 position);

	glm::vec3 GetFront() const;
	void SetFront(glm::vec3 front);

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);

	void UpdateView();

private:
	glm::vec3 mCameraPos = glm::vec3(0.0f, 0.0f, -10.0f);
	glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 mCameraUp = glm::vec3(0.0f, -1.0f, 0.0f);

	glm::mat4 mView = glm::identity<glm::mat4>();

	float mCameraSpeed = 0.1f; // adjust accordingly

	Renderer& mRenderer;
};
