#pragma once

class EntityRegistry;

class PhysicsSystem
{
public:
	void Update(float deltaTime, EntityRegistry& entityRegistry) const;
};
