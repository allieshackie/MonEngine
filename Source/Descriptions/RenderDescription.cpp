#include "Components/RenderComponent.h"

#include "RenderDescription.h"

void RenderDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<RenderComponent>(entity);
}

void RenderDescription::ParseJSON(const nlohmann::json& json)
{
	/*
	 *
	auto vertices = json[VERTICES_STRING];

	for (auto vertex : vertices)
	{
		auto position = vertex[POSITION_STRING];
		auto color = vertex[COLOR_STRING];
		auto texCoord = vertex[TEXCOORD_STRING];

		mVertices.push_back({
			{position[0], position[1], position[2]}, {color[0], color[1], color[2]}, {texCoord[0], texCoord[1]}
		});
	}
	 */
}
