#include "Entity/Components/ScriptComponent.h"

#include "ScriptDescription.h"

void ScriptDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	ScriptComponent script;
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		script.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "ScriptComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<ScriptComponent>(script);
}
