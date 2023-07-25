#pragma once

class CollisionSystem;
class EntityRegistry;
class MapSystem;

class MapCollisionSystem
{
public:
	MapCollisionSystem(CollisionSystem& collisionSystem, EntityRegistry& entityRegistry, MapSystem& mapSystem);
	~MapCollisionSystem() = default;

	void Update();

private:
	void _CheckForCollision();

	CollisionSystem& mCollisionSystem;
	EntityRegistry& mEntityRegistry;
	MapSystem& mMapSystem;
};
