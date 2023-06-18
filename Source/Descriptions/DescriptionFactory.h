#pragma once

#include "CollisionDescription.h"
#include "DescriptionBase.h"
#include "InteractiveDescription.h"
#include "PhysicsDescription.h"
#include "PlayerDescription.h"
#include "RenderDescription.h"
#include "SpriteDescription.h"
#include "TransformDescription.h"

class DescriptionFactory
{
public:
	DescriptionFactory() = default;
	~DescriptionFactory() = default;

	std::shared_ptr<DescriptionBase> CreateDescription(const std::string& descriptionName,
	                                                   const nlohmann::json& json) const
	{
		if (descriptionName == RenderDescription::JsonName)
		{
			const auto render = std::make_shared<RenderDescription>();
			render->ParseJSON(json);
			return render;
		}

		if (descriptionName == SpriteDescription::JsonName)
		{
			const auto sprite = std::make_shared<SpriteDescription>();
			sprite->ParseJSON(json);
			return sprite;
		}

		if (descriptionName == TransformDescription::JsonName)
		{
			const auto transform = std::make_shared<TransformDescription>();
			transform->ParseJSON(json);
			return transform;
		}

		if (descriptionName == InteractiveDescription::JsonName)
		{
			const auto interactive = std::make_shared<InteractiveDescription>();
			interactive->ParseJSON(json);
			return interactive;
		}

		if (descriptionName == PlayerDescription::JsonName)
		{
			const auto player = std::make_shared<PlayerDescription>();
			player->ParseJSON(json);
			return player;
		}


		if (descriptionName == PhysicsDescription::JsonName)
		{
			const auto physics = std::make_shared<PhysicsDescription>();
			physics->ParseJSON(json);
			return physics;
		}

		if (descriptionName == CollisionDescription::JsonName)
		{
			const auto collision = std::make_shared<CollisionDescription>();
			collision->ParseJSON(json);
			return collision;
		}

		std::cout << "tried to create an undefined description: " << descriptionName << std::endl;
		assert(false);
		return nullptr;
	}
};
