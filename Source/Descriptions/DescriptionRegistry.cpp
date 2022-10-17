#include "DescriptionRegistry.h"
#include "TileSetDescription.h"

void DescriptionRegistry::registerAllDescriptions()
{
	registerDescription<TileSetDescription>(TileSetDescription::JsonName);
}

template <class TDescription>
void DescriptionRegistry::registerDescription(const std::string& descriptionName)
{
	auto factory = std::make_unique<TDescription>("");
	mRegisteredDescriptions.insert(std::pair(descriptionName, std::move(factory)));
}

template <class TDescription>
TDescription* DescriptionRegistry::getDescription(const std::string& descriptionName)
{
	const auto description = mRegisteredDescriptions.find(descriptionName);
	if (description == mRegisteredDescriptions.end())
	{
		return nullptr;
	}
	return dynamic_cast<TDescription*>(description->second);
}
