#pragma once

#include "DescriptionBase.h"

class DescriptionRegistry {
public:
	template<class TDescription>
	static void registerDescription(const std::string& descriptionName);

	template<class TDescription>
	static TDescription* getDescription(const std::string& descriptionName);

	static void registerAllDescriptions();

private:
	static std::unordered_map<std::string, DescriptionBase*> mRegisteredDescriptions;
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
