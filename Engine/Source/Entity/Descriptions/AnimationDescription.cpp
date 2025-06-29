#include "Entity/Components/AnimationComponent.h"

#include "AnimationDescription.h"

void AnimationDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	AnimationComponent anim;
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		anim.load(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "AnimationComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<AnimationComponent>(anim);
}
