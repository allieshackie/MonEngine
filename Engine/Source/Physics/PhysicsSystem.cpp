#include "Core/EngineContext.h"
#include "Core/Scene.h"
#include "Core/SceneManager.h"
#include "Entity/Entity.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(EngineContext& engineContext)
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
	//mDynamicWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb);
}

void PhysicsSystem::RegisterCollider(Entity* entity, const ResourceManager& resourceManager)
{
	const auto& transform = entity->GetComponent<TransformComponent>();
	auto& collider = entity->GetComponent<CollisionComponent>();
	const auto& mesh = entity->GetComponent<MeshComponent>();

	const auto physics = entity->TryGetComponent<PhysicsComponent>();
	const auto& position = transform.mPosition;
	const auto& size = transform.mSize;
	const auto& rotation = transform.mRotation;
	const auto& model = resourceManager.GetModelFromId(mesh.mMeshPath);
	auto calculatedSize = model.CalculateModelScaling(size);

	// If physics component exists, dynamic collider
	if (physics != nullptr)
	{
		if (collider.mColliderShape == ColliderShapes::BOX)
		{
			btScalar mass(physics->mMass);
			btVector3 localInertia(0, 0, 0);
			auto capsuleShape = new btCapsuleShape(calculatedSize.x * 2, calculatedSize.y * 3);
			capsuleShape->calculateLocalInertia(mass, localInertia);

			// Set the initial position and rotation
			btTransform boxTransform;
			boxTransform.setIdentity();
			boxTransform.setOrigin(btVector3{position.x, position.y, position.z});
			boxTransform.setRotation(_ConvertDegreesToQuat(rotation));

			auto motionState = new btDefaultMotionState(boxTransform);
			const btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, capsuleShape, localInertia);

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
			const auto modelMesh = model.GetMeshes()[0];
			auto triangleMesh = new btTriangleMesh();
			for (int i = 0; i < modelMesh->mIndices.size(); i += 3)
			{
				const auto& v0 = modelMesh->mVertices[modelMesh->mIndices[i]].position * calculatedSize;
				const auto& v1 = modelMesh->mVertices[modelMesh->mIndices[i + 1]].position * calculatedSize;
				const auto& v2 = modelMesh->mVertices[modelMesh->mIndices[i + 2]].position * calculatedSize;
				triangleMesh->addTriangle({v0.x, v0.y, v0.z}, {v1.x, v1.y, v1.z}, {v2.x, v2.y, v2.z});
			}

			auto triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true);

			btTransform boxTransform;
			boxTransform.setIdentity();
			boxTransform.setOrigin(btVector3{position.x, position.y, position.z});
			boxTransform.setRotation(_ConvertDegreesToQuat(rotation));

			const auto motionState = new btDefaultMotionState(boxTransform);
			const btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, triangleMeshShape, {0, 0, 0});

			collider.mColliderIndex = mDynamicWorld->getNumCollisionObjects();
			auto rigidBody = new btRigidBody(rbInfo);
			mDynamicWorld->addRigidBody(rigidBody);

			collider.mRigidBody = rigidBody;
		}
	}
}

void PhysicsSystem::Update(float deltaTime, MonScene* scene, const ResourceManager& resourceManager)
{
	for (auto it = mEntitiesToInitialize.begin(); it != mEntitiesToInitialize.end();)
	{
		const auto& collider = (*it)->GetComponent<CollisionComponent>();
		if (collider.mInitialized)
		{
			RegisterCollider(*it, resourceManager);
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

			transform.mPosition = {pos.x(), pos.y() - (size.y / 2), pos.z()};
			transform.mRotation = _ConvertQuatToRadians(orn);
		}
	});
}

void PhysicsSystem::SetSceneCallbacks(const SceneManager& sceneManager)
{
	EventFunc func = [this](Entity* entity)
	{
		AddEntityToInitialize(entity);
	};
	sceneManager.ConnectOnConstruct<CollisionComponent>(func);
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
