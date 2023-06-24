#pragma once
#include <functional>

class EventPublisher
{
private:
	class EventSubscription;
	using EventFunc = std::function<void(int, const std::type_info&)>;
	using PublishList = std::vector<std::shared_ptr<EventSubscription>>;
	using PublishMap = std::unordered_map<std::string, PublishList>;
public:
	EventPublisher() = default;
	~EventPublisher() = default;

	std::shared_ptr<EventSubscription> AddListener(const std::string& eventType, EventFunc& callback);
	void RemoveListener(const EventSubscription& sub);

	void Notify(const std::string& eventType, int entityId, const std::type_info& typeInfo);

private:
	PublishMap mList;

	class EventSubscription
	{
	public:
		EventSubscription(EventFunc& handlerFunc) : mHandlerFunc(std::move(handlerFunc))
		{
		}

		void Remove()
		{
			if (mIterator != mOwner->mList[mEventType].end())
			{
				mOwner->mList[mEventType].erase(mIterator);
				mIterator = mOwner->mList[mEventType].end();
			}
		}

	private:
		EventPublisher* mOwner = nullptr;
		EventFunc mHandlerFunc;

		std::string mEventType;
		PublishList::iterator mIterator;

		friend class EventPublisher;
	};
};
