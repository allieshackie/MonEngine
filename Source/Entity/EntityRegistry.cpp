#include "EntityRegistry.h"
#include "EntityContext.h"

MonEntityRegistry::MonEntityRegistry() {

}

MonEntityRegistry::~MonEntityRegistry() {

}

EntityContext MonEntityRegistry::CreateEntity() {
	EntityContext entity = { mRegistry, mRegistry.create() };
	return entity;
}

void MonEntityRegistry::RemoveEntity(EntityId id)
{
	mRegistry.destroy(id);
}