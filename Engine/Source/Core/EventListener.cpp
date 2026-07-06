#include "EventListener.h"

const EventFunc& EventSubscription::GetHandlerFunc() const
{
	return mHandlerFunc;
}

void EventPublisher::RemoveListener(const std::string& eventType, SubscriptionHandle handle)
{
	auto it = mList.find(eventType);
	if (it != mList.end())
	{
		auto& listeners = it->second;
		listeners.erase(
			std::remove_if(listeners.begin(), listeners.end(),
				[handle](const std::shared_ptr<EventSubscription>& sub) { return sub->GetHandle() == handle; }),
			listeners.end()
		);
	}
}

void EventPublisher::Flush()
{
	for (const auto& event : mQueueEvents)
	{
		const auto it = mList.find(event.eventType);
		if (it != mList.end())
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

	mQueueEvents.clear();
}

void EventPublisher::AddWorldCreatedListener(WorldFunc callback)
{
	mWorldListeners.push_back(callback);
}	

void EventPublisher::NotifyWorldCreated(std::weak_ptr<World> world)
{
	for (auto& func : mWorldListeners)
	{
		func(world);
	}
}

