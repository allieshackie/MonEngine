#include "Core/EngineContext.h"
#include "Core/Scene.h"
#include "Entity/Entity.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem()
{
	mBroadPhase = std::make_unique<btDbvtBroadphase>();
	mConstraintSolver = std::make_unique<btSequentialImpulseConstraintSolver>();

	// create
	mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	mCollisionDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());
	mDynamicWorld = std::make_unique<btDiscreteDynamicsWorld>(mCollisionDispatcher.get(),
	                                                          mBroadPhase.get(),
	                                                          mConstraintSolver.get(),
	                                                          mCollisionConfiguration.get());

	// setup
	mDynamicWorld->setGravity(mGravityConst);

	// TODO: Uncomment to turn on debug draw
	//mPhysicsDebugDraw = std::make_unique<PhysicsDebugDraw>(engineContext);
	//mDynamicWorld->setDebugDrawer(mPhysicsDebugDraw.get());
	//mDynamicWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

void PhysicsSystem::RegisterCollider(Entity* entity)
{
	const auto& transform = entity->GetComponent<TransformComponent>();
	auto& collider = entity->GetComponent<CollisionComponent>();

	const auto physics = entity->TryGetComponent<PhysicsComponent>();

	// If physics component exists, dynamic collider
	if (physics != nullptr)
	{
		if (collider.mColliderShape == ColliderShapes::BOX)
		{
			const auto& position = transform.mPosition;
			const auto& size = transform.mSize;
			const auto& rotation = transform.mRotation;
			auto boxShape = new btBoxShape({size.x, size.y, size.z});

			btTransform boxTransform;
			boxTransform.setIdentity();

			btScalar mass(physics->mMass);
			btVector3 localInertia(0, 0, 0);
			boxShape->calculateLocalInertia(mass, localInertia);

			// Set the initial position and rotation
			// TODO: Get proper model size, size is currently "scale"
			boxTransform.setOrigin(btVector3{position.x, position.y + size.y, position.z});
			boxTransform.setRotation(_ConvertDegreesToQuat(rotation));

			auto motionState = new btDefaultMotionState(boxTransform);
			const btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, boxShape, localInertia);

			collider.mColliderIndex = mDynamicWorld->getNumCollisionObjects();
			collider.mIsDynamic = true;

			auto rigidBody = new btRigidBody(rbInfo);
			rigidBody->setDamping(0.8f, 0.6f);
			rigidBody->setAngularFactor(btVector3(0, 0, 0));
			mDynamicWorld->addRigidBody(rigidBody);
			collider.mRigidBody = rigidBody;
		}
	}
	else
	{
		if (collider.mColliderShape == ColliderShapes::BOX)
		{
			const auto& position = transform.mPosition;
			const auto& size = transform.mSize;
			const auto& rotation = transform.mRotation;
			const auto boxShape = new btBoxShape(btVector3{size.x, size.y, size.z});

			btTransform boxTransform;
			boxTransform.setIdentity();

			const btVector3 localInertia(0, 0, 0);

			boxTransform.setOrigin(btVector3{position.x, position.y, position.z});
			boxTransform.setRotation(_ConvertDegreesToQuat(rotation));

			const auto motionState = new btDefaultMotionState(boxTransform);
			const btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, boxShape, localInertia);

			collider.mColliderIndex = mDynamicWorld->getNumCollisionObjects();
			auto rigidBody = new btRigidBody(rbInfo);
			mDynamicWorld->addRigidBody(rigidBody);

			collider.mRigidBody = rigidBody;
		}
	}
}

void PhysicsSystem::Update(float deltaTime, MonScene* scene)
{
	for (auto it = mEntitiesToInitialize.begin(); it != mEntitiesToInitialize.end();)
	{
		const auto& collider = (*it)->GetComponent<CollisionComponent>();
		if (collider.mInitialized)
		{
			RegisterCollider(*it);
			it = mEntitiesToInitialize.erase(it);
		}
		else
		{
			// Move to the next element
			++it;
		}
	}

	mDynamicWorld->stepSimulation(deltaTime, 10, 1.0f / 60.0f);
	mDynamicWorld->debugDrawWorld();

	if (scene == nullptr) return;
	const auto view = scene->GetRegistry().view<CollisionComponent, TransformComponent>();
	view.each([=](auto& collider, auto& transform)
	{
		if (collider.mIsDynamic && collider.mRigidBody != nullptr)
		{
			const auto& size = transform.mSize;
			btRigidBody* body = collider.mRigidBody;

			btTransform worldTransform;
			body->getMotionState()->getWorldTransform(worldTransform);

			btVector3 pos = worldTransform.getOrigin();
			btQuaternion orn = worldTransform.getRotation();

			transform.mPosition = {pos.x(), pos.y() - size.y, pos.z()};
			transform.mRotation = _ConvertQuatToRadians(orn);
		}
	});
}

void PhysicsSystem::SetSceneCallbacks(const MonScene* scene)
{
	EventFunc func = [this](Entity* entity)
	{
		AddEntityToInitialize(entity);
	};
	scene->ConnectOnConstruct<CollisionComponent>(func);
}

btQuaternion PhysicsSystem::_ConvertDegreesToQuat(glm::vec3 rot)
{
	const auto radianConversion = glm::radians(rot);
	btQuaternion q;
	q.setEuler(radianConversion.y, radianConversion.x, radianConversion.z);

	return q;
}

glm::vec3 PhysicsSystem::_ConvertQuatToRadians(btQuaternion quat)
{
	glm::vec3 radians;
	quat.getEulerZYX(radians.z, radians.y, radians.x);
	const auto degreeConversion = glm::degrees(radians);
	return degreeConversion;
}

void PhysicsSystem::AddEntityToInitialize(Entity* entity)
{
	mEntitiesToInitialize.push_back(entity);
}
