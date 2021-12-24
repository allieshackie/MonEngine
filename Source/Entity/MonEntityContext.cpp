#include "EntityRegistry.h"

#include "MonEntityContext.h"

MonEntityContext::MonEntityContext(EnTTRegistry& registry, EntityId id) : mRegistry(registry), mId(id) {

}

MonEntityContext::~MonEntityContext() {

}

EntityId MonEntityContext::getId() const
{
	return mId;
}
