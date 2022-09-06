#pragma once

#include "DescriptionBase.h"

struct SpriteDescription : ComponentDescriptionBase {
public:
	SpriteDescription();

	void ApplyToEntity(EntityContext& entity) override {}
    void ParseJSON(const nlohmann::json& jsonFrag) override;

public:
	static constexpr char JsonName[] = "mondev:sprite";
private:
	static constexpr char RESOURCE_PATH_STRING[] = "resource_path";

};