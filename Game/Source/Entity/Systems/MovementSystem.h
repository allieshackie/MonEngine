#pragma once

class EntityRegistry;
class PhysicsSystem;

class MovementSystem
{
public:
	void Update(EntityRegistry& registry, PhysicsSystem& physicsSystem);
};
