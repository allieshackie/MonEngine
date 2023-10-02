#pragma once
#include <functional>

class EventSubscription;
using EventFunc = std::function<void(int, const std::type_info&)>;
using PublishList = std::vector<std::shared_ptr<EventSubscription>>;
using PublishMap = std::unordered_map<std::string, PublishList>;

class EventPublisher
{
public:
	void AddListener(const std::string& eventType, EventFunc& callback);
	//void RemoveListener(const std::unique_ptr<EventSubscription>& sub);

	void Notify(const std::string& eventType, int entityId, const std::type_info& typeInfo);


private:
	PublishMap mList;
};

class EventSubscription
{
public:
	EventSubscription(EventFunc handlerFunc) : mHandlerFunc(std::move(handlerFunc))
	{
	}

	EventSubscription(EventFunc handlerFunc, std::string eventType, PublishList::iterator it) : mHandlerFunc(
		std::move(handlerFunc)), mEventType(std::move(eventType)), mIterator(it)
	{
	}

	const EventFunc& GetHandlerFunc() const;
	const std::string& GetEventType() const;
	const PublishList::iterator& GetPublishListIterator() const;

	void SetEventType(const std::string& event);
	void SetPublishListIterator(const PublishList::iterator it);

private:
	EventFunc mHandlerFunc;

	std::string mEventType;
	PublishList::iterator mIterator;
};
