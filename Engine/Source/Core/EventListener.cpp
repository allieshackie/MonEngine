#include "EventListener.h"

void EventPublisher::Notify(const std::string& eventType, std::type_index  componentType, entt::entity entity)
{
	mEntityQueueEvents.push_back({ std::string(eventType), componentType, entity });
}

void EventPublisher::Notify(std::weak_ptr<World> world)
{
	for (auto& func : mWorldListeners)
	{
		func(world);
	}
}

void EventPublisher::Notify(PhysicsEventType type, entt::entity entityA, entt::entity entityB)
{
	mPhysicsQueueEvents.push_back({ type, entityA, entityB });
}

SubscriptionHandle EventPublisher::AddListener(const std::string& eventType, std::type_index componentType, EntityEventFunc& callback)
{
	SubscriptionHandle handle = mNextHandle++;
	auto sub = std::make_shared<EntityEventSubscription>(callback, eventType, componentType, handle);
	mEntityListeners[eventType].push_back(sub);
	return handle;
}

void EventPublisher::RemoveListener(const std::string& eventType, SubscriptionHandle handle)
{
	auto it = mEntityListeners.find(eventType);
	if (it != mEntityListeners.end())
	{
		auto& listeners = it->second;
		listeners.erase(
			std::remove_if(listeners.begin(), listeners.end(),
				[handle](const std::shared_ptr<EntityEventSubscription>& sub) { return sub->GetHandle() == handle; }),
			listeners.end()
		);
	}
}

SubscriptionHandle EventPublisher::AddListener(PhysicsEventType type, PhysicsEventFunc& callback)
{
	SubscriptionHandle handle = mNextHandle++;
	auto sub = std::make_shared<PhysicsEventSubscription>(callback, type, handle);
	mPhysicsListeners[type].push_back(sub);
	return handle;
}

void EventPublisher::RemoveListener(PhysicsEventType type, SubscriptionHandle handle)
{
	auto it = mPhysicsListeners.find(type);
	if (it != mPhysicsListeners.end())
	{
		auto& listeners = it->second;
		listeners.erase(
			std::remove_if(listeners.begin(), listeners.end(),
				[handle](const std::shared_ptr<PhysicsEventSubscription>& sub) { return sub->GetHandle() == handle; }),
			listeners.end()
		);
	}
}

void EventPublisher::AddWorldCreatedListener(WorldFunc callback)
{
	mWorldListeners.push_back(callback);
}

void EventPublisher::Flush()
{
	for (const auto& event : mEntityQueueEvents)
	{
		const auto it = mEntityListeners.find(event.eventType);
		if (it != mEntityListeners.end())
		{
			const auto& eventListeners = it->second;
			for (const auto& listener : eventListeners)
			{
				if (event.componentType == listener->GetType())
				{
					listener->GetHandlerFunc()(event.entity);
				}
			}
		}
	}

	mEntityQueueEvents.clear();

	for (const auto& event : mPhysicsQueueEvents)
	{
		const auto it = mPhysicsListeners.find(event.type);
		if (it != mPhysicsListeners.end())
		{
			const auto& eventListeners = it->second;
			for (const auto& listener : eventListeners)
			{
				if (event.type == listener->GetType())
				{
					listener->GetHandlerFunc()(event.entityA, event.entityB);
				}
			}
		}
	}

	mPhysicsQueueEvents.clear();
}

