#pragma once

class EntityRegistry;

class PhysicsSystem
{
public:
	PhysicsSystem(EntityRegistry& entityRegistry);
	~PhysicsSystem() = default;

	void Update(float deltaTime);

private:
	EntityRegistry& mEntityRegistry;
};
