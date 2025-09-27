#pragma once

class CollisionSystem;
class MonScene;
class MapSystem;

class MapCollisionSystem
{
public:
	MapCollisionSystem(CollisionSystem& collisionSystem, MapSystem& mapSystem);
	~MapCollisionSystem() = default;

	void Update(MonScene* scene);

private:
	void _CheckForCollision();

	CollisionSystem& mCollisionSystem;
	MapSystem& mMapSystem;
};
