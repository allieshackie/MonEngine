#pragma once
#include <glm/vec3.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>

#include "Graphics/Animation/Animation.h"

enum class ColliderShapes : int
{
	BOX = 0,
	SPHERE,
	CAPSULE,
	TRI_MESH,
	CONVEX
};

inline std::map<std::string, ColliderShapes> COLLIDER_NAMES = {
	{"Box", ColliderShapes::BOX},
	{"Sphere", ColliderShapes::SPHERE},
	{"Capsule", ColliderShapes::CAPSULE},
	{"TriMesh", ColliderShapes::TRI_MESH},
	{"Convex", ColliderShapes::CONVEX},
};

inline std::map<ColliderShapes, std::string> COLLIDER_STRINGS = {
	{ColliderShapes::BOX, "Box"},
	{ColliderShapes::SPHERE, "Sphere"},
	{ColliderShapes::CAPSULE, "Capsule"},
	{ColliderShapes::TRI_MESH, "TriMesh"},
	{ColliderShapes::CONVEX, "Convex"},
};

namespace cereal
{
	template <class Archive>
	void serialize(Archive& archive, glm::vec2& v) { archive(v.x, v.y); }

	template <class Archive>
	void serialize(Archive& archive, glm::vec3& v) { archive(v.x, v.y, v.z); }

	template <class Archive>
	void serialize(Archive& archive, glm::vec4& v) { archive(v.x, v.y, v.z, v.w); }

	template <>
	struct specialize<JSONInputArchive, ColliderShapes, cereal::specialization::non_member_load_save>
	{
	};

	template <>
	struct specialize<JSONOutputArchive, ColliderShapes, cereal::specialization::non_member_load_save>
	{
	};

	// Save function for writing the enum as a string
	template <class Archive>
	void save(Archive& archive, const ColliderShapes& state)
	{
		archive(COLLIDER_STRINGS[state]);
	}

	// Load function for reading a string and converting it to an enum
	template <class Archive>
	void load(Archive& archive, ColliderShapes& state)
	{
		std::string stateStr;
		archive(stateStr);
		state = COLLIDER_NAMES[stateStr];
	}

	// Optionally load an NVP if its name equals to the current node's name
	// Loading members should be done in the same order they were saved
	// return true if NVP found
	template <class Archive, class T>
	bool make_optional_nvp(Archive& ar, const char* name, T&& value)
	{
		const auto node_name = ar.getNodeName();

		// if names are equal
		if (node_name != nullptr &&
			strcmp(name, node_name) == 0)
		{
			ar(make_nvp(name, std::forward<T>(value))); // load the NVP. Advances to the next node
			return true;
		}

		return false;
	}

	template <class Archive, class T>
	void make_optional_nvp(OutputArchive<Archive>& ar, const char* name, T&& value)
	{
		ar(make_nvp(name, std::forward<T>(value)));
	}

	// Saves NVP if predicate is true. Useful for avoiding splitting into save & load if also saving optionally.
	template <class Archive, class T, class Predicate>
	void make_optional_nvp(OutputArchive<Archive>& ar, const char* name, T&& value, Predicate predicate)
	{
		if (predicate())
			ar(make_nvp(name, std::forward<T>(value)));
	}

	template <class Archive, class T, class Predicate>
	bool make_optional_nvp(Archive& ar, const char* name, T&& value, Predicate predicate)
	{
		return make_optional_nvp(ar, name, std::forward<T>(value));
	}
} // namespace cereal
