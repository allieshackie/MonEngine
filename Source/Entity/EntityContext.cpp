#include "EntityRegistry.h"
#include "EntityContext.h"

EntityContext::EntityContext(EnTTRegistry& registry, EntityId id) : mRegistry(registry), mId(id) {

}

EntityContext::~EntityContext() {

}

EntityId EntityContext::getId()
{
	return mId;
}
