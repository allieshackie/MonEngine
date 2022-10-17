#pragma once

#include "DescriptionBase.h"

class DescriptionRegistry
{
public:
	DescriptionRegistry() = default;
	~DescriptionRegistry() = default;

	template <class TDescription>
	void registerDescription(const std::string& descriptionName);

	template <class TDescription>
	TDescription* getDescription(const std::string& descriptionName);

	void registerAllDescriptions();

private:
	std::unordered_map<std::string, std::unique_ptr<DescriptionBase>> mRegisteredDescriptions;
};
