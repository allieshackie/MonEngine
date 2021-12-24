#include "DescriptionRegistry.h"
#include "TileSetDescription.h"
#include "MapDescription.h"

DescriptionRegistry::DescriptionRegistry()
{
	registerDescription<TileSetDescription>(TileSetDescription::JsonName);
	registerDescription<MapDescription>(MapDescription::JsonName);
}
