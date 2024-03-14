#pragma once

#include "Entity/Descriptions/DescriptionBase.h"

class MeshDescription : public DescriptionBase
{
public:
	MeshDescription() = default;
	// Copy constructor/assignment operator
	MeshDescription(const MeshDescription& other) = default;
	MeshDescription& operator=(const MeshDescription& other) = default;
	// Move constructor/assignment operator
	MeshDescription(MeshDescription&& other) noexcept = default;
	MeshDescription& operator=(MeshDescription&& rhs) noexcept = default;

	~MeshDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;
	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "mesh";

private:
	static constexpr char MESH_PATH_STRING[] = "mesh_path";

	std::string mMeshPath;
};
