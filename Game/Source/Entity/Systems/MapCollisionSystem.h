#pragma once

class CollisionSystem;
class MapSystem;

class MapCollisionSystem
{
public:
	MapCollisionSystem(CollisionSystem& collisionSystem, MapSystem& mapSystem);
	~MapCollisionSystem() = default;

	void Update();

private:
	void _CheckForCollision();

	CollisionSystem& mCollisionSystem;
	MapSystem& mMapSystem;
};
