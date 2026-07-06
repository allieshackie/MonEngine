#include "Entity/Components/SpriteComponent.h"
#include "Util/SerialUtil.h"

#include "SpriteDescription.h"

void SpriteDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	SpriteComponent& sprite = entity->AddComponent<SpriteComponent>();
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		sprite.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "SpriteComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}
}
