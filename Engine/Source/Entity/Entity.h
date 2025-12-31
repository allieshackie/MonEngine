#pragma once
#include <entt/entt.hpp>
#include "Core/EventListener.h"
#include "Util/LuaUtil.h"

class Entity
{
public:
	Entity(entt::entity id, entt::registry& registry, EventPublisher& eventPub, std::string name = "")
		: mRegistry(registry), mEventPublisher(eventPub), mId(id), mName(std::move(name))
	{
	}

	const entt::entity& GetId() const { return mId; }
	const std::string& GetName() const { return mName; }
	void SetName(const std::string& name) { mName = name; }

	template <typename Component>
	Component& GetComponent()
	{
		return mRegistry.get<Component>(mId);
	}

	template <typename Component>
	Component* TryGetComponent()
	{
		return mRegistry.try_get<Component>(mId);
	}

	template <typename Component, typename... Args>
	void AddComponentWithArgs(Args&&... args)
	{
		mRegistry.emplace<Component>(mId, std::forward<Args>(args)...);
		mEventPublisher.Notify<Component>("on_construct", this);
	}

	template <typename Component>
	void AddComponent(Component component)
	{
		mRegistry.emplace<Component>(mId, component);
		mEventPublisher.Notify<Component>("on_construct", this);
	}

	template <typename Component, typename... Args>
	Component& AddOrGetComponent(Args&&... args)
	{
		return mRegistry.emplace_or_replace<Component>(mId, std::forward<Args>(args)...);
	}

	template <typename Component>
	void RemoveComponent() const
	{
		mRegistry.remove<Component>(mId);
		mEventPublisher.Notify<Component>("on_destroy", this);
	}

	template <typename... Components>
	bool HasComponent() const
	{
		return mRegistry.all_of<Components>(mId);
	}

	static void Bind(lua_State* state)
	{
		luaL_newmetatable(state, LuaName);

		LuaUtil::RegisterMethod<Entity, &Entity::GetName>(state, "GetName");

		lua_pushvalue(state, -1);
		lua_setfield(state, -2, "__index");

		lua_pop(state, 1);
	}

	static constexpr char LuaName[] = "Entity";
private:
	entt::registry& mRegistry;
	EventPublisher& mEventPublisher;

	entt::entity mId{entt::null};
	std::string mName;
};
