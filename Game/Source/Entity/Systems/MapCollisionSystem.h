#pragma once

class CollisionSystem;
class MapSystem;
class World;

class MapCollisionSystem
{
public:
	MapCollisionSystem(CollisionSystem& collisionSystem, MapSystem& mapSystem, std::weak_ptr<World> world);
	~MapCollisionSystem() = default;

	void Update(float dt);

private:
	void _CheckForCollision();

	CollisionSystem& mCollisionSystem;
	MapSystem& mMapSystem;
	std::weak_ptr<World> mWorld;
};
