#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/CollisionComponent.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/Node.h"
#include "Graphics/Core/ResourceManager.h"

#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(RenderSystem& renderSystem, ResourceManager& resourceManager, EventPublisher& eventPublisher)
	: mResourceManager(resourceManager)
{
	mBroadPhase = std::make_unique<btDbvtBroadphase>();
	mConstraintSolver = std::make_unique<btSequentialImpulseConstraintSolver>();

	mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	mCollisionDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());
	mDynamicWorld = std::make_unique<btDiscreteDynamicsWorld>(mCollisionDispatcher.get(),
	                                                          mBroadPhase.get(),
	                                                          mConstraintSolver.get(),
	                                                          mCollisionConfiguration.get());

	mDynamicWorld->setGravity(mGravityConst);

	eventPublisher.AddWorldCreatedListener(
		[this](std::weak_ptr<World> world) {
			if (const auto worldShared = world.lock())
			{
				EventFunc func = [this](Entity* entity)
					{
						AddEntityToInitialize(entity);
					};
				worldShared->ConnectOnConstruct<CollisionComponent>(func);
			}
			mWorld = world;
		}
	);

	// Uncomment to turn on debug draw
	//mPhysicsDebugDraw = std::make_unique<PhysicsDebugDraw>(renderSystem);
	//mDynamicWorld->setDebugDrawer(mPhysicsDebugDraw.get());
	//mDynamicWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

void PhysicsSystem::RegisterCollider(Entity* entity)
{
	const auto& transform = entity->GetComponent<TransformComponent>();
	auto& collider = entity->GetComponent<CollisionComponent>();
	const auto& mesh = entity->GetComponent<ModelComponent>();

	const auto physics = entity->TryGetComponent<PhysicsComponent>();
	const auto& position = transform.mPosition;
	const auto& size = transform.mSize;
	const auto& model = mResourceManager.GetModelFromId(mesh.mModelPath);
	glm::vec3 worldBounds = model.CalculateWorldBounds(size) + collider.mSize;
	glm::vec3 halfExtents = worldBounds * 0.5f;
	glm::vec3 modelCenter = (model.GetMaxBounds() + model.GetMinBounds()) * 0.5f;
	glm::vec3 scale = model.CalculateModelScaling(size);
	glm::vec3 scaledCenter = modelCenter * scale;

	btCollisionShape* shape = nullptr;

	switch (collider.mColliderShape)
	{
	case ColliderShapes::BOX:
		shape = new btBoxShape(btVector3{ halfExtents.x, halfExtents.y, halfExtents.z});
		break;
	case ColliderShapes::SPHERE:
		shape = new btSphereShape(halfExtents.x);
		break;
	case ColliderShapes::CAPSULE:
		shape = new btCapsuleShape(halfExtents.x, worldBounds.y - (halfExtents.x * 2.0f));
		break;
	case ColliderShapes::CONVEX:
		{
			auto convex = new btConvexHullShape();
			for (const auto& modelMesh : model.GetMeshes())
			{
				for (const auto& v : modelMesh->mVertices)
				{
					auto convexPos = btVector3(v.position.x, v.position.y - halfExtents.y, v.position.z) * btVector3(
						halfExtents.x, halfExtents.y, halfExtents.z);
					convex->addPoint(convexPos);
				}
			}
			shape = convex;
			break;
		}
	case ColliderShapes::TRI_MESH:
		{
			const auto meshes = model.GetMeshes();
			auto triangleMesh = new btTriangleMesh();
			for (const auto& mesh : meshes)
			{
				const auto& verts = mesh->mVertices;
				const auto& indices = mesh->mIndices;
				for (int i = 0; i + 2 < indices.size(); i += 3)
				{
					auto& v0 = verts[indices[i]].position;
					auto& v1 = verts[indices[i + 1]].position;
					auto& v2 = verts[indices[i + 2]].position;

					triangleMesh->addTriangle(
						btVector3(v0.x * scale.x, v0.y * scale.y, v0.z * scale.z),
						btVector3(v1.x * scale.x, v1.y * scale.y, v1.z * scale.z),
						btVector3(v2.x * scale.x, v2.y * scale.y, v2.z * scale.z)
					);
				}
			}

			auto baseShape = new btBvhTriangleMeshShape(triangleMesh, true);
			shape = baseShape;
			break;
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

	float originY = collider.mColliderShape == ColliderShapes::TRI_MESH ? position.y : position.y - (scaledCenter.y / 2);

	btTransform boxTransform;
	boxTransform.setIdentity();
	boxTransform.setOrigin(btVector3(
		position.x + scaledCenter.x,
		originY,
		position.z + scaledCenter.z
	));
	boxTransform.setRotation(transform.ToBulletQuat());

	const auto motionState = new btDefaultMotionState(boxTransform);
	const btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	collider.mColliderIndex = mDynamicWorld->getNumCollisionObjects();
	auto rigidBody = new btRigidBody(rbInfo);
	if (physics != nullptr)
	{
		rigidBody->setFriction(1.0f);
		rigidBody->setDamping(0.9f, 0.9f);
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
			++it;
		}
	}

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
				transform.mPosition = {pos.x(), pos.y() - (size.y / 2), pos.z()};
			}
		});
	}
}

void PhysicsSystem::Update(float dt)
{
	mDynamicWorld->stepSimulation(dt, 10, 1.0f / 60.0f);
	mDynamicWorld->debugDrawWorld();
}

btQuaternion PhysicsSystem::_ConvertDegreesToQuat(glm::vec3 rot)
{
	const auto radianConversion = glm::radians(rot);
	btQuaternion q;
	q.setEuler(radianConversion.y, radianConversion.x, radianConversion.z);

	return q;
}

void PhysicsSystem::AddEntityToInitialize(Entity* entity)
{
	mEntitiesToInitialize.push_back(entity);
}
