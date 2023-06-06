#pragma once
#include <entt/entt.hpp>
#include "EntityTemplateRegistry.h"

class MonEntityContext;

struct EntityId
{
	using entity_type = std::uint32_t;
	static constexpr auto null = entt::null;

	constexpr EntityId(entity_type value = null) noexcept
		: entt{value}
	{
	}

	constexpr EntityId(const EntityId& other) noexcept
		: entt{other.entt}
	{
	}

	constexpr operator entity_type() const noexcept
	{
		return entt;
	}

private:
	entity_type entt;
};

using EnTTRegistry = entt::basic_registry<EntityId>;

class EntityRegistry
{
public:
	EntityRegistry();
	~EntityRegistry() = default;
	EntityRegistry(const EntityRegistry& other) = delete;
	void operator=(const EntityRegistry&) = delete;

	EnTTRegistry& GetEnttRegistry();

	EntityId CreateEntityFromTemplate(const char* templateName);
	EntityId CreateEntity();
	void RemoveEntity(EntityId id);

	template <typename T>
	T& GetComponent(EntityId id)
	{
		return mRegistry.get<T>(id);
	}

	template <typename T, typename... Args>
	void AddComponent(EntityId id, Args&&... args)
	{
		mRegistry.emplace<T>(id, std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	T& AddOrGetComponent(EntityId id, Args&&... args)
	{
		return mRegistry.emplace_or_replace<T>(id, std::forward<Args>(args)...);
	}

	template <typename Component>
	void RemoveComponent(EntityId id)
	{
		mRegistry.remove<Component>(id);
	}

	template <typename... Components>
	bool HasComponent(EntityId id) const
	{
		return mRegistry.all_of<Components>(id);
	}

	//auto view = registry.view<const position, velocity>();

	// use a callback
	//view.each([](const auto& pos, auto& vel) { /* ... */ });

	// use an extended callback
	//view.each([](const auto entity, const auto& pos, auto& vel) { /* ... */ });

	// use a range-for
	//for (auto [entity, pos, vel] : view.each()) {
	// ...
	//}

	// use forward iterators and get only the components of interest
	//for (auto entity : view) {
	//	auto& vel = view.get<velocity>(entity);
	// ...
	//  }

private:
	EnTTRegistry mRegistry;
	std::unique_ptr<EntityTemplateRegistry> mEntityTemplateRegistry;
};
