#pragma once
#include <unordered_set>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

#include "PhysicsDebugDraw.h"
#include "Core/ISystem.h"

class Entity;
class EventPublisher;
class RenderSystem;
class ResourceManager;
class World;

struct PhysicsObject
{
	std::unique_ptr<btCollisionShape> mShape = nullptr;
	std::unique_ptr<btMotionState> mMotionState = nullptr;
	std::unique_ptr<btRigidBody> mRigidBody = nullptr;
	std::unique_ptr<btTriangleMesh> mTriangleMesh = nullptr;
};

struct TriggerObject
{
	std::unique_ptr<btCollisionShape> mShape = nullptr;
	std::unique_ptr<btGhostObject> mGhostObject = nullptr;
	std::unordered_set<Entity*> mPreviousOverlaps;
};

class PhysicsSystem : public ISystem
{
public:
	PhysicsSystem(RenderSystem& renderSystem, ResourceManager& resourceManager, EventPublisher& eventPublisher);

	void FixedUpdate(float dt) override;
	void Update(float dt) override;

	btRigidBody* GetRigidbody(entt::entity entityId);
	bool Raycast(btVector3 start, btVector3 end);

	void Flush();

private:
	btQuaternion _ConvertDegreesToQuat(glm::vec3 rot);

	void _RegisterPhysicsBody(entt::entity entityId);
	void _RegisterCollider(entt::entity entityId, Entity* entity);
	void _RegisterTrigger(entt::entity entityId, Entity* entity);

	void _UpdateTriggers();

	std::unique_ptr<btBroadphaseInterface> mBroadPhase;
	std::unique_ptr<btCollisionDispatcher> mCollisionDispatcher;
	std::unique_ptr<btSequentialImpulseConstraintSolver> mConstraintSolver;
	std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
	std::unique_ptr<btDiscreteDynamicsWorld> mDynamicWorld;

	// world properties
	btVector3 mGravityConst = {0.0f, -9.81f, 0.0f};

	std::unordered_map<entt::entity, PhysicsObject> mPhysicsObjects;
	std::unordered_map<entt::entity, TriggerObject> mTriggerObjects;

	std::unique_ptr<PhysicsDebugDraw> mPhysicsDebugDraw;
	std::vector<entt::entity> mEntitiesToInitialize;
	ResourceManager& mResourceManager;
	std::weak_ptr<World> mWorld;
	EventPublisher& mEventPublisher;

	bool mQueueFlush = false;
	const float kMinCapsuleHeight = 1.0f;
};
