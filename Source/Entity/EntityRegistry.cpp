#include "MonEntityContext.h"

#include "EntityRegistry.h"


MonEntityRegistry::MonEntityRegistry() {

}

MonEntityRegistry::~MonEntityRegistry() {

}

MonEntityContext MonEntityRegistry::CreateEntity() {
	MonEntityContext entity = { mRegistry, mRegistry.create() };
	return entity;
}

void MonEntityRegistry::RemoveEntity(EntityId id)
{
	mRegistry.destroy(id);
}