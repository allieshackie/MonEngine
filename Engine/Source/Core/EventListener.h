#pragma once
#include <functional>

class EventSubscription;
using EventFunc = std::function<void(int, const std::type_info&)>;
using PublishList = std::vector<std::shared_ptr<EventSubscription>>;
using PublishMap = std::unordered_map<std::string, PublishList>;

class EventPublisher
{
public:
	EventPublisher() = default;
	~EventPublisher() = default;

	std::shared_ptr<EventSubscription> AddListener(const std::string& eventType, EventFunc& callback);
	void RemoveListener(const std::shared_ptr<EventSubscription>& sub);

	void Notify(const std::string& eventType, int entityId, const std::type_info& typeInfo);


private:
	PublishMap mList;
};

class EventSubscription
{
public:
	EventSubscription(EventFunc& handlerFunc) : mHandlerFunc(std::move(handlerFunc))
	{
	}

private:
	EventFunc mHandlerFunc;

	std::string mEventType;
	PublishList::iterator mIterator;

	friend class EventPublisher;
};
