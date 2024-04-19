#include "Core/EngineContext.h"
#include "Entity/EntityRegistry.h"
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
}

void PhysicsSystem::UpdateCollisionShapes(EntityRegistry& entityRegistry)
{
	const auto staticView = entityRegistry.GetEnttRegistry().view<
		CollisionComponent, TransformComponent>(entt::exclude<PhysicsComponent>);
	const auto dynamicView = entityRegistry.GetEnttRegistry().view<
		CollisionComponent, TransformComponent, PhysicsComponent>();

	// Static RigidBody
	staticView.each([=](auto& staticCollider, const auto& staticTransform)
	{
		if (staticCollider.mColliderShape == ColliderShapes::Box)
		{
			const auto& position = staticTransform.mPosition;
			const auto& size = staticTransform.mSize;
			const auto& rotation = staticTransform.mRotation;
			const auto boxShape = new btBoxShape(btVector3{size.x, size.y, size.z});

			btTransform boxTransform;
			boxTransform.setIdentity();
			boxTransform.setOrigin(btVector3{position.x, position.y, position.z});
			boxTransform.setRotation(_ConvertDegreesToQuat(rotation));

			const btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, boxShape, btVector3{0, 0, 0});

			staticCollider.mColliderIndex = mDynamicWorld->getNumCollisionObjects();
			mDynamicWorld->addRigidBody(new btRigidBody(rbInfo));
		}
	});
	// Dynamic rigidbody
	dynamicView.each([=](auto& collider, const auto& transform, auto& physics)
	{
		if (collider.mColliderShape == ColliderShapes::Box)
		{
			const auto& position = transform.mPosition;
			const auto& size = transform.mSize;
			const auto& rotation = transform.mRotation;
			auto boxShape = new btBoxShape({size.x, size.y, size.z});

			btTransform boxTransform;
			boxTransform.setIdentity();

			btScalar mass(1.f);
			btVector3 localInertia(0, 0, 0);
			boxShape->calculateLocalInertia(mass, localInertia);

			boxTransform.setOrigin(btVector3{position.x, position.y, position.z});
			boxTransform.setRotation(_ConvertDegreesToQuat(rotation));

			auto motionState = new btDefaultMotionState(boxTransform);
			const btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, boxShape, localInertia);

			collider.mColliderIndex = mDynamicWorld->getNumCollisionObjects();
			mDynamicWorld->addRigidBody(new btRigidBody(rbInfo));
		}
	});
}

void PhysicsSystem::Update(float deltaTime, EntityRegistry& entityRegistry)
{
	mDynamicWorld->stepSimulation(deltaTime);

	const auto view = entityRegistry.GetEnttRegistry().view<CollisionComponent, TransformComponent>();
	view.each([=](const auto& collider, auto& transform)
	{
		int collisionIndex = collider.mColliderIndex;

		if (collisionIndex < mDynamicWorld->getNumCollisionObjects())
		{
			btCollisionObject* obj = mDynamicWorld->getCollisionObjectArray()[collisionIndex];
			btVector3 pos = obj->getWorldTransform().getOrigin();
			btQuaternion orn = obj->getWorldTransform().getRotation();

			transform.mPosition = {pos.x(), pos.y(), pos.z()};
			transform.mRotation = _ConvertQuatToRadians(orn);
		}
	});

	// TODO: Debug draw the bounds of the colliders to check if box is falling around ground
	/*
	 *
	//print positions of all objects
	for (int i = 0; i < mDynamicWorld->getNumCollisionObjects(); i++)
	{
		btCollisionObject* obj = mDynamicWorld->getCollisionObjectArray()[i];
		btVector3 pos = obj->getWorldTransform().getOrigin();
		btQuaternion orn = obj->getWorldTransform().getRotation();

		//engine.DrawBox({static_cast<float>(pos.x()), static_cast<float>(pos.y()), static_cast<float>(pos.z())},
		//               {1, 1, 1}, {1, 1, 1, 1}, false);
		//printf("world pos object %d = %f,%f,%f\n", j, pos.getX(),
		//     pos.getY(), pos.getZ());
	}
	 */
}

btQuaternion PhysicsSystem::_ConvertDegreesToQuat(glm::vec3 rot)
{
	btQuaternion q;
	const auto radianConversion = glm::radians(rot);
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
