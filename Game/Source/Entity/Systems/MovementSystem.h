#pragma once

#include "Core/ISystem.h"

class PhysicsSystem;
class World;

class MovementSystem : public ISystem
{
public:
	MovementSystem(PhysicsSystem& physicsSystem, EventPublisher& eventPublisher);

	void Update(float dt) override;

private:
	PhysicsSystem& mPhysicsSystem;
	std::weak_ptr<World> mWorld;
};
