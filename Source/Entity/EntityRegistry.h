#pragma once
#include <entt/entt.hpp>

class EntityContext;

using EntityId = uint32_t;
using EnTTRegistry = entt::basic_registry<EntityId>;

class MonEntityRegistry {
public:
	MonEntityRegistry(const MonEntityRegistry& other) = delete;

	void operator=(const MonEntityRegistry&) = delete;

	EntityContext CreateEntity();

	void RemoveEntity(EntityId id);

protected:
	MonEntityRegistry();
	~MonEntityRegistry();

private:
	EnTTRegistry mRegistry;
};