#include "Descriptions/DescriptionBase.h"

#include "EntityRegistry.h"

EntityRegistry::EntityRegistry(DescriptionFactory& descriptionFactory, EventPublisher& eventPublisher)
	: mEventPublisher(eventPublisher)
{
	mEntityTemplateRegistry = std::make_unique<EntityTemplateRegistry>(descriptionFactory);
}

EnTTRegistry& EntityRegistry::GetEnttRegistry()
{
	return mRegistry;
}

EntityId EntityRegistry::CreateEntityFromTemplate(const char* templateName)
{
	const auto& descriptions = mEntityTemplateRegistry->GetEntityTemplateDescriptions(templateName);
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

void EntityRegistry::RemoveEntity(const EntityId& id)
{
	mRegistry.destroy(id);
}
