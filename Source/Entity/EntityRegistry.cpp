#include "DescriptionBase.h"
#include "EntityTemplateRegistry.h"

#include "EntityRegistry.h"

EntityRegistry::EntityRegistry(EntityTemplateRegistry& entityTemplateRegistry)
	: mEntityTemplateRegistry(entityTemplateRegistry)
{
}

EnTTRegistry& EntityRegistry::GetEnttRegistry()
{
	return mRegistry;
}

EntityId EntityRegistry::CreateEntityFromTemplate(const char* templateName)
{
	const auto descriptions = mEntityTemplateRegistry.GetEntityTemplateDescriptions(templateName);
	const auto entity = CreateEntity();

	for (const auto& description : descriptions)
	{
		description->ApplyToEntity(entity, *this);
	}

	return entity;
}

EntityId EntityRegistry::CreateEntity()
{
	return mRegistry.create();
}

void EntityRegistry::RemoveEntity(const EntityId id)
{
	mRegistry.destroy(id);
}
