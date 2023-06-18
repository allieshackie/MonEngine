#pragma once
#include <functional>

class EventListener
{
public:
	using EventCallback = std::function<void(int, const std::type_info&)>;

	EventListener() = default;
	~EventListener() = default;

	void AddListener(const std::string& eventType, const EventCallback& callback);
	void RemoveListener(const std::string& eventType, const EventCallback& callback);

	void Notify(const std::string& eventType, int entityId, const std::type_info& typeInfo);

private:
	std::unordered_map<std::string, std::vector<EventCallback>> mListeners;
};
