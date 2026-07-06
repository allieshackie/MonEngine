#pragma once
#include <functional>
#include <typeindex>
#include <entt/entt.hpp>

class EventSubscription;
class World;

using EventFunc = std::function<void(entt::entity)>;
using PublishList = std::vector<std::shared_ptr<EventSubscription>>;
using PublishMap = std::unordered_map<std::string, PublishList>;
using SubscriptionHandle = uint32_t;

using WorldFunc = std::function<void(std::weak_ptr<World>)>;
using WorldPublishList = std::vector<WorldFunc>;

struct EventEntry {
	std::string eventType;
	std::type_index componentType;
	entt::entity entity;
};

class EventPublisher
{
public:
	template <typename Component>
	SubscriptionHandle AddListener(const std::string& eventType, EventFunc& callback);
	void RemoveListener(const std::string& eventType, SubscriptionHandle handle);

	template <typename Component>
	void Notify(const std::string& eventType, entt::entity entity);

	void Flush();

	void AddWorldCreatedListener(WorldFunc callback);
	void NotifyWorldCreated(std::weak_ptr<World> world);

private:
	PublishMap mList;
	SubscriptionHandle mNextHandle = 0;

	WorldPublishList mWorldListeners;

	std::vector<EventEntry> mQueueEvents;
};

class EventSubscription
{
public:
	EventSubscription(EventFunc handlerFunc, std::string eventType, std::type_index type, SubscriptionHandle handle) 
		: mHandlerFunc(std::move(handlerFunc)), mEventType(std::move(eventType)), mType(type), mHandle(handle)
	{
	}

	const EventFunc& GetHandlerFunc() const;
	const std::type_index& GetType() const { return mType; }
	SubscriptionHandle GetHandle() const { return mHandle; }

private:
	EventFunc mHandlerFunc;
	std::string mEventType;
	std::type_index mType;
	SubscriptionHandle mHandle;
};

template <typename Component>
SubscriptionHandle EventPublisher::AddListener(const std::string& eventType, EventFunc& callback)
{
	SubscriptionHandle handle = mNextHandle++;
	auto sub = std::make_shared<EventSubscription>(callback, eventType, std::type_index(typeid(Component)), handle);
	mList[eventType].push_back(sub);
	return handle;
}

template <typename Component>
void EventPublisher::Notify(const std::string& eventType, entt::entity entity)
{
	mQueueEvents.push_back({ std::string(eventType), std::type_index(typeid(Component)), entity });
}
