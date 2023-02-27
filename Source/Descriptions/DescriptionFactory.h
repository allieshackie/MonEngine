#pragma once

#include "DescriptionBase.h"
#include "RenderDescription.h"
#include "MapDescription.h"
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
		if (descriptionName == MapDescription::JsonName)
		{
			const auto map = std::make_shared<MapDescription>();
			map->ParseJSON(json);
			return map;
		}

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

		std::cout << "tried to create an undefined description: " << descriptionName << std::endl;
		assert(false);
		return nullptr;
	}
};
