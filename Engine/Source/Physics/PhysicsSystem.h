#pragma once

class EntityRegistry;

class PhysicsSystem
{
public:
	PhysicsSystem() = default;
	~PhysicsSystem() = default;

	void Update(float deltaTime, EntityRegistry& entityRegistry) const;
};
