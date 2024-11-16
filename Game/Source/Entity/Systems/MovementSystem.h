#pragma once

class Camera;
class EntityRegistry;
class PhysicsSystem;

class MovementSystem
{
public:
	void Update(EntityRegistry& registry, PhysicsSystem& physicsSystem, const Camera& camera);
};
