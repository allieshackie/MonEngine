#include "EventListener.h"

std::shared_ptr<EventSubscription> EventPublisher::AddListener(const std::string& eventType,
                                                               EventFunc& callback)
{
	auto sub = std::make_shared<EventSubscription>(callback);
	mList[eventType].push_back(sub);

	sub->mEventType = eventType;
	sub->mIterator = std::prev(mList[eventType].end());

	return sub;
}

void EventPublisher::RemoveListener(const std::shared_ptr<EventSubscription>& sub)
{
	const auto it = mList.find(sub->mEventType);
	if (it != mList.end())
	{
		auto& eventListeners = it->second;
		eventListeners.erase(sub->mIterator);
	}
}

void EventPublisher::Notify(const std::string& eventType, int entityId, const std::type_info& typeInfo)
{
	const auto it = mList.find(eventType);
	if (it != mList.end())
	{
		const auto& eventListeners = it->second;
		for (const auto& listener : eventListeners)
		{
			listener->mHandlerFunc(entityId, typeInfo);
		}
	}
}
