#include "Entity/Components/AnimationComponent.h"

#include "AnimationDescription.h"

void AnimationDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	std::istringstream jsonStream(mJson);

	AnimationComponent anim;
	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		anim.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "AnimationComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<AnimationComponent>(anim);
}
