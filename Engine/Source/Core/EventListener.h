#pragma once
#include <functional>
#include <typeindex>

class Entity;
class EventSubscription;
using EventFunc = std::function<void(Entity*)>;
using PublishList = std::vector<std::shared_ptr<EventSubscription>>;
using PublishMap = std::unordered_map<std::string, PublishList>;

class EventPublisher
{
public:
	template <typename Component>
	void AddListener(const std::string& eventType, EventFunc& callback);
	//void RemoveListener(const std::unique_ptr<EventSubscription>& sub);

	template <typename Component>
	void Notify(const std::string& eventType, Entity* entity);

private:
	PublishMap mList;
};

class EventSubscription
{
public:
	EventSubscription(EventFunc handlerFunc, std::string eventType, std::type_index type) :
		mHandlerFunc(std::move(handlerFunc)),
		mEventType(std::move(eventType)),
		mType(type)
	{
	}

	const EventFunc& GetHandlerFunc() const;
	const std::type_index& GetType() const { return mType; }

private:
	EventFunc mHandlerFunc;
	std::string mEventType;
	std::type_index mType;
};

template <typename Component>
void EventPublisher::AddListener(const std::string& eventType, EventFunc& callback)
{
	auto sub = std::make_shared<EventSubscription>(callback, eventType, typeid(Component));
	mList[eventType].push_back(sub);
}

template <typename Component>
void EventPublisher::Notify(const std::string& eventType, Entity* entity)
{
	const auto it = mList.find(eventType);
	if (it != mList.end())
	{
		const auto& eventListeners = it->second;
		for (const auto& listener : eventListeners)
		{
			if (std::type_index(typeid(Component)) == listener->GetType())
			{
				listener->GetHandlerFunc()(entity);
			}
		}
	}
}
