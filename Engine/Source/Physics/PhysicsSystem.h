#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

class EngineContext;
class EntityRegistry;

class PhysicsSystem
{
public:
	PhysicsSystem();

	void UpdateCollisionShapes(EntityRegistry& entityRegistry);
	void Update(float deltaTime, EntityRegistry& entityRegistry);

private:
	btQuaternion _ConvertDegreesToQuat(glm::vec3 rot);
	glm::vec3 _ConvertQuatToRadians(btQuaternion quat);

	std::unique_ptr<btBroadphaseInterface> mBroadPhase;
	std::unique_ptr<btCollisionDispatcher> mCollisionDispatcher;
	std::unique_ptr<btSequentialImpulseConstraintSolver> mConstraintSolver;
	std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
	std::unique_ptr<btDiscreteDynamicsWorld> mDynamicWorld;
	//std::unique_ptr<btSoftBodyWorldInfo> mWorldInfo;
	//PhysicsDebugDraw* debug_draw = nullptr;

	// world properties
	btVector3 mGravityConst = {0.0f, -9.81f, 0.0f};
};
