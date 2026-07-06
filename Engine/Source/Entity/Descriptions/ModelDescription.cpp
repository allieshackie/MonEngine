#include "Entity/Components/ModelComponent.h"

#include "ModelDescription.h"

void ModelDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	ModelComponent& model = entity->AddComponent<ModelComponent>();
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		model.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "ModelComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}
}
