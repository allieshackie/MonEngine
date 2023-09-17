#include "EventListener.h"

std::shared_ptr<EventSubscription> EventPublisher::AddListener(const std::string& eventType,
                                                               EventFunc& callback)
{
	auto sub = std::make_shared<EventSubscription>(callback);
	mList[eventType].push_back(sub);

	sub->SetEventType(eventType);
	sub->SetPublishListIterator(std::prev(mList[eventType].end()));

	return sub;
}

void EventPublisher::RemoveListener(const std::shared_ptr<EventSubscription>& sub)
{
	const auto it = mList.find(sub->GetEventType());
	if (it != mList.end())
	{
		auto& eventListeners = it->second;
		eventListeners.erase(sub->GetPublishListIterator());
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
			listener->GetHandlerFunc()(entityId, typeInfo);
		}
	}
}

const EventFunc& EventSubscription::GetHandlerFunc() const
{
	return mHandlerFunc;
}

const std::string& EventSubscription::GetEventType() const
{
	return mEventType;
}

const PublishList::iterator& EventSubscription::GetPublishListIterator() const
{
	return mIterator;
}

void EventSubscription::SetEventType(const std::string& event)
{
	mEventType = event;
}

void EventSubscription::SetPublishListIterator(const PublishList::iterator it)
{
	mIterator = it;
}
