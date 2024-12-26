#pragma once
#include <entt/entity/entity.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

class SceneManager;

class Camera
{
public:
	Camera(SceneManager& entityRegistry, glm::vec3 position, glm::vec3 front, glm::vec3 up);

	void Update(SceneManager& entityRegistry);

	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();

	void ZoomIn();
	void ZoomOut();

	const glm::mat4& GetView() const { return mView; }
	const glm::vec3& GetPosition() const { return mCameraPos; }
	const glm::vec3& GetFront() const { return mCameraFront; }
	const glm::vec3& GetRight() const { return mCameraRight; }

	void ToggleFollowCam() { mFollowCam = !mFollowCam; }
	void SetPosition(const glm::vec3 pos);
	void SetFront(const glm::vec3 front);
	void SetUp(const glm::vec3 up);
	void UpdateRight();
	void SetLookTarget(entt::registry& registry, entt::entity id);

	void UpdateView();

private:
	entt::entity mCameraTargetEntity{entt::null};
	glm::vec3 mCameraTarget = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraFollowOffset = {0.0f, 10.0f, -20.0f};
	glm::vec3 mCameraLookOffset = {0.0f, 6.0f, -1.0f};

	glm::vec3 mCameraPos = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraFront = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraUp = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraRight = {0.0f, 0.0f, 0.0f};

	glm::mat4 mView = glm::identity<glm::mat4>();

	float mCameraSpeed = 0.1f; // adjust accordingly
	bool mFollowCam = false;
};
