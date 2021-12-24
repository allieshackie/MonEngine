#pragma once

#include "DescriptionBase.h"

class DescriptionRegistry {
public:
	DescriptionRegistry();
	DescriptionRegistry(const DescriptionRegistry& other) = delete;
	DescriptionRegistry(DescriptionRegistry&& other) = default;
	virtual ~DescriptionRegistry() = default;

	template<class TDescription>
	void registerDescription(const std::string& descriptionName);

	template<class TDescription>
	TDescription* getDescription(const std::string& descriptionName);

private:
	std::unordered_map<std::string, DescriptionBase*> mRegisteredDescriptions;
};

template<class TDescription>
void DescriptionRegistry::registerDescription(const std::string& descriptionName)
{
	TDescription* factory = new TDescription();
	mRegisteredDescriptions.insert(std::pair(descriptionName, factory));
}

template<class TDescription>
TDescription* DescriptionRegistry::getDescription(const std::string& descriptionName)
{
	auto description = mRegisteredDescriptions.find(descriptionName);
	if (description == mRegisteredDescriptions.end()) {
		return nullptr;
	}
	return dynamic_cast<TDescription*>(description->second);
}
