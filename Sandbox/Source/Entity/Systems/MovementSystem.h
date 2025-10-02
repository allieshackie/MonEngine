#pragma once

class PhysicsSystem;
class World;

class MovementSystem : public ISystem
{
public:
	MovementSystem(PhysicsSystem& physicsSystem, std::weak_ptr<World> world);

	void Update(float dt) override;

private:
	PhysicsSystem& mPhysicsSystem;
	std::weak_ptr<World> mWorld;
};
