#pragma once

class Camera;
class MonScene;
class PhysicsSystem;

class MovementSystem
{
public:
	void Update(MonScene* scene, PhysicsSystem& physicsSystem, const Camera& camera);
};
