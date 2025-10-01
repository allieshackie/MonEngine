#pragma once

class PhysicsSystem;
class World;

class MovementSystem
{
public:
	MovementSystem(PhysicsSystem& physicsSystem, std::weak_ptr<World> world);

	void Update(float dt);

private:
	PhysicsSystem& mPhysicsSystem;
	std::weak_ptr<World> mWorld;
};
