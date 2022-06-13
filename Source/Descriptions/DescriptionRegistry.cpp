#include "DescriptionRegistry.h"
#include "TileSetDescription.h"
#include "MapDescription.h"

std::unordered_map<std::string, DescriptionBase*> DescriptionRegistry::mRegisteredDescriptions;

void DescriptionRegistry::registerAllDescriptions()
{
	registerDescription<TileSetDescription>(TileSetDescription::JsonName);
	registerDescription<MapDescription>(MapDescription::JsonName);
}
