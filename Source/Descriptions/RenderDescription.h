#pragma once

#include "Core/Vertex.h"
#include "DescriptionBase.h"

struct RenderDescription : DescriptionBase
{
public:
	RenderDescription() = default;
	~RenderDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;

	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "render";

private:
	static constexpr char VERTICES_STRING[] = "vertices";
	static constexpr char POSITION_STRING[] = "position";
	static constexpr char COLOR_STRING[] = "color";
	static constexpr char TEXCOORD_STRING[] = "texture_coordinates";

	std::vector<Vertex> mVertices;
};
