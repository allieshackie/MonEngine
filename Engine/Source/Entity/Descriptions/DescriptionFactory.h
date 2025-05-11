#pragma once
#include <nlohmann/json.hpp>
#include "DescriptionBase.h"

class DescriptionFactory
{
public:
	using DescriptionCreationFunction = std::function<std::shared_ptr<DescriptionBase>()>;

	template <typename T>
	void RegisterDescription(const std::string& descriptionName)
	{
		DescriptionCreationFunction func = []() { return std::make_shared<T>(); };
		mDescriptionRegistry[descriptionName] = std::move(func);
	}

	std::shared_ptr<DescriptionBase> CreateDescription(const std::string& descriptionName,
	                                                   const std::string& json) const
	{
		const auto it = mDescriptionRegistry.find(descriptionName);
		if (it != mDescriptionRegistry.end())
		{
			// Call the creation function to create a new component
			auto description = it->second();
			description->SaveJson(json);
			return description;
		}

		// Component with the given ID not found
		std::cout << "tried to create an undefined description: " << descriptionName << std::endl;
		assert(false);
		return nullptr;
	}

private:
	std::unordered_map<std::string, DescriptionCreationFunction> mDescriptionRegistry;
};
