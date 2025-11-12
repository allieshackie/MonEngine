#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/Node.h"
#include "Graphics/Core/ResourceManager.h"

#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(RenderSystem& renderSystem, ResourceManager& resourceManager, std::weak_ptr<World> world)
	: mResourceManager(resourceManager), mWorld(world)
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

	if (const auto worldShared = mWorld.lock())
	{
		EventFunc func = [this](Entity* entity)
		{
			AddEntityToInitialize(entity);
		};
		worldShared->ConnectOnConstruct<CollisionComponent>(func);
	}

	// Uncomment to turn on debug draw
	//mPhysicsDebugDraw = std::make_unique<PhysicsDebugDraw>(renderSystem);
	//mDynamicWorld->setDebugDrawer(mPhysicsDebugDraw.get());
	//mDynamicWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb);
}

void PhysicsSystem::RegisterCollider(Entity* entity)
{
	const auto& transform = entity->GetComponent<TransformComponent>();
	auto& collider = entity->GetComponent<CollisionComponent>();
	const auto& mesh = entity->GetComponent<ModelComponent>();

	const auto physics = entity->TryGetComponent<PhysicsComponent>();
	const auto& position = transform.mPosition;
	const auto& size = transform.mSize;
	const auto& rotation = transform.mRotation;
	const auto& model = mResourceManager.GetModelFromId(mesh.mModelPath);
	auto calculatedSize = model.CalculateModelScaling(size);

	btCollisionShape* shape = nullptr;

	switch (collider.mColliderShape)
	{
	case ColliderShapes::BOX:
		shape = new btBoxShape(btVector3{size.x / 2, size.y / 2, size.z / 2});
		break;
	case ColliderShapes::SPHERE:
		shape = new btSphereShape(calculatedSize.x * 2);
		break;
	case ColliderShapes::CAPSULE:
		shape = new btCapsuleShape(calculatedSize.x * 2, calculatedSize.y * 3);
		break;
	case ColliderShapes::CONVEX:
		{
			auto convex = new btConvexHullShape();
			for (const auto& modelMesh : model.GetMeshes())
			{
				for (const auto& v : modelMesh->mVertices)
				{
					auto convexPos = btVector3(v.position.x, v.position.y - size.y, v.position.z) * btVector3(
						calculatedSize.x, calculatedSize.y, calculatedSize.z);
					convex->addPoint(convexPos);
				}
			}
			shape = convex;
			break;
		}
	case ColliderShapes::TRI_MESH:
		{
			const auto modelMesh = model.GetMeshes()[0];
			auto triangleMesh = new btTriangleMesh();
			for (int i = 0; i < modelMesh->mIndices.size(); i += 3)
			{
				const auto& v0 = modelMesh->mVertices[modelMesh->mIndices[i]].position * calculatedSize;
				const auto& v1 = modelMesh->mVertices[modelMesh->mIndices[i + 1]].position * calculatedSize;
				const auto& v2 = modelMesh->mVertices[modelMesh->mIndices[i + 2]].position * calculatedSize;
				triangleMesh->addTriangle({v0.x, v0.y, v0.z}, {v1.x, v1.y, v1.z}, {v2.x, v2.y, v2.z});
			}

			shape = new btBvhTriangleMeshShape(triangleMesh, true);
		}
	}

	btScalar mass(0);
	btVector3 localInertia(0, 0, 0);

	// If physics component exists, dynamic collider
	if (physics != nullptr)
	{
		mass = physics->mMass;
		shape->calculateLocalInertia(mass, localInertia);
		collider.mIsDynamic = true;
	}

	btTransform boxTransform;
	boxTransform.setIdentity();
	boxTransform.setOrigin(btVector3{position.x, position.y + (size.y / 2), position.z});
	boxTransform.setRotation(_ConvertDegreesToQuat(rotation));

	const auto motionState = new btDefaultMotionState(boxTransform);
	const btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	collider.mColliderIndex = mDynamicWorld->getNumCollisionObjects();
	auto rigidBody = new btRigidBody(rbInfo);
	if (physics != nullptr)
	{
		rigidBody->setDamping(0.8f, 0.6f);
		rigidBody->setAngularFactor(btVector3(0, 0, 0));
	}
	mDynamicWorld->addRigidBody(rigidBody);

	collider.mRigidBody = rigidBody;
}

void PhysicsSystem::FixedUpdate(float dt)
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

	mDynamicWorld->stepSimulation(dt, 10, 1.0f / 60.0f);
	mDynamicWorld->debugDrawWorld();

	if (const auto world = mWorld.lock())
	{
		const auto view = world->GetRegistry().view<CollisionComponent, TransformComponent>();
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

				transform.mPosition = {pos.x(), pos.y() - (size.y / 2), pos.z()};
				transform.mRotation = _ConvertQuatToRadians(orn);
			}
		});
	}
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
