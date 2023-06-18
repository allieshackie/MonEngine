#include "EventListener.h"

void EventListener::AddListener(const std::string& eventType, const EventCallback& callback)
{
	mListeners[eventType].push_back(callback);
}

void EventListener::RemoveListener(const std::string& eventType, const EventCallback& callback)
{
	const auto it = mListeners.find(eventType);
	if (it != mListeners.end())
	{
		auto& eventListeners = it->second;
		eventListeners.erase(std::remove(eventListeners.begin(), eventListeners.end(), callback), eventListeners.end());
	}
}

void EventListener::Notify(const std::string& eventType, int entityId, const std::type_info& typeInfo)
{
	const auto it = mListeners.find(eventType);
	if (it != mListeners.end())
	{
		const auto& eventListeners = it->second;
		for (const auto& listener : eventListeners)
		{
			listener(entityId, typeInfo);
		}
	}
}
