#include "Entity/Components/SpriteComponent.h"
#include "Util/SerialUtil.h"

#include "SpriteDescription.h"

void SpriteDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	std::istringstream jsonStream(mJson);
	SpriteComponent sprite;

	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		sprite.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "SpriteComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<SpriteComponent>(sprite);
}
