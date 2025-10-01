#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "PhysicsDebugDraw.h"

class Entity;
class RenderSystem;
class ResourceManager;
class World;

class PhysicsSystem
{
public:
	PhysicsSystem(RenderSystem& renderSystem, ResourceManager& resourceManager, std::weak_ptr<World> world);

	void RegisterCollider(Entity* entity);
	void AddEntityToInitialize(Entity* entity);

	void Update(float dt);

	void SetSceneCallbacks();

	btDiscreteDynamicsWorld& GetDynamicWorld() const { return *mDynamicWorld; }

private:
	btQuaternion _ConvertDegreesToQuat(glm::vec3 rot);
	glm::vec3 _ConvertQuatToRadians(btQuaternion quat);

	std::unique_ptr<btBroadphaseInterface> mBroadPhase;
	std::unique_ptr<btCollisionDispatcher> mCollisionDispatcher;
	std::unique_ptr<btSequentialImpulseConstraintSolver> mConstraintSolver;
	std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
	std::unique_ptr<btDiscreteDynamicsWorld> mDynamicWorld;

	// world properties
	btVector3 mGravityConst = {0.0f, -9.81f, 0.0f};

	std::unique_ptr<PhysicsDebugDraw> mPhysicsDebugDraw;
	std::vector<Entity*> mEntitiesToInitialize;
	ResourceManager& mResourceManager;
	std::weak_ptr<World> mWorld;
};
