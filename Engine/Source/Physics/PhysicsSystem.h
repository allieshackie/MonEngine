#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "PhysicsDebugDraw.h"

class EngineContext;
class EntityRegistry;

class PhysicsSystem
{
public:
	PhysicsSystem(EngineContext& engineContext);

	void RegisterCollider(EnTTRegistry& registry, EntityId entity);
	void Update(float deltaTime, EntityRegistry& entityRegistry);

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
};
