#pragma once
#include <glm/vec3.hpp>
#include "Entity/EntityRegistry.h"

class PlayerSystem
{
public:
	PlayerSystem(EntityRegistry& entityRegistry);
	~PlayerSystem() = default;

	void SpawnPlayer(glm::vec3 spawnPoint);

	glm::vec3 GetPlayerPosition() const;
	void SetPlayerPosition(glm::vec3 newPosition) const;

private:
	void _MoveForward(bool set) const;
	void _MoveBackward(bool set) const;
	void _MoveLeft(bool set) const;
	void _MoveRight(bool set) const;

	EntityId mPlayerEntity;

	EntityRegistry& mEntityRegistry;
};
