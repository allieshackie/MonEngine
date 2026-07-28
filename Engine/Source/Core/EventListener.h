#pragma once
#include <functional>
#include <typeindex>
#include <entt/entt.hpp>

class EntityEventSubscription;
class PhysicsEventSubscription;
class World;

using SubscriptionHandle = uint32_t;

struct EntityEventEntry
{
	std::string eventType;
	std::type_index componentType;
	entt::entity entity;
};

enum class PhysicsEventType
{
	CollisionEnter,
	CollisionStay,
	CollisionExit,

	TriggerEnter,
	TriggerStay,
	TriggerExit
};

struct PhysicsEventEntry
{
	PhysicsEventType type;
	entt::entity entityA;
	entt::entity entityB;
};

using EntityEventFunc = std::function<void(entt::entity)>;

using EntityPublishList = std::vector<std::shared_ptr<EntityEventSubscription>>;
using EntityPublishMap = std::unordered_map<std::string, EntityPublishList>;

using PhysicsEventFunc = std::function<void(entt::entity, entt::entity)>;
using PhysicsPublishList = std::vector<std::shared_ptr<PhysicsEventSubscription>>;
using PhysicsPublishMap = std::unordered_map<PhysicsEventType, PhysicsPublishList>;

using WorldFunc = std::function<void(std::weak_ptr<World>)>;
using WorldPublishList = std::vector<WorldFunc>;

class EventPublisher
{
public:
	EventPublisher() = default;

	EventPublisher(const EventPublisher&) = delete;
	EventPublisher& operator=(const EventPublisher&) = delete;

	EventPublisher(EventPublisher&&) = delete;
	EventPublisher& operator=(EventPublisher&&) = delete;

	SubscriptionHandle AddListener(const std::string& eventType, std::type_index componentType, EntityEventFunc& callback);
	void RemoveListener(const std::string& eventType, SubscriptionHandle handle);

	SubscriptionHandle AddListener(PhysicsEventType type, PhysicsEventFunc& callback);
	void RemoveListener(PhysicsEventType type, SubscriptionHandle handle);

	void Notify(const std::string& eventType, std::type_index componentType, entt::entity entity); // Entity
	void Notify(std::weak_ptr<World> world); // World
	void Notify(PhysicsEventType type, entt::entity entityA, entt::entity entityB); // Physics

	void Flush();

	void AddWorldCreatedListener(WorldFunc callback);

private:
	SubscriptionHandle mNextHandle = 0;

	EntityPublishMap mEntityListeners;
	PhysicsPublishMap mPhysicsListeners;
	WorldPublishList mWorldListeners;

	std::vector<EntityEventEntry> mEntityQueueEvents;
	std::vector<PhysicsEventEntry> mPhysicsQueueEvents;
};

class EntityEventSubscription
{
public:
	EntityEventSubscription(EntityEventFunc handlerFunc, std::string eventType, std::type_index type, SubscriptionHandle handle)
		: mHandlerFunc(std::move(handlerFunc)), mEventType(std::move(eventType)), mType(type), mHandle(handle)
	{}

	const EntityEventFunc& GetHandlerFunc() const { return mHandlerFunc; }
	const std::type_index& GetType() const { return mType; }
	SubscriptionHandle GetHandle() const { return mHandle; }

private:
	EntityEventFunc mHandlerFunc;
	std::string mEventType;
	std::type_index mType;
	SubscriptionHandle mHandle;
};

class PhysicsEventSubscription
{
public:
	PhysicsEventSubscription(PhysicsEventFunc handlerFunc, PhysicsEventType type, SubscriptionHandle handle)
		: mHandlerFunc(std::move(handlerFunc)), mType(type), mHandle(handle)
	{}

	const PhysicsEventFunc& GetHandlerFunc() const { return mHandlerFunc; }
	PhysicsEventType GetType() const { return mType; }
	SubscriptionHandle GetHandle() const { return mHandle; }

private:
	PhysicsEventFunc mHandlerFunc;
	PhysicsEventType mType;
	SubscriptionHandle mHandle;
};
