#pragma once
#include <entt/entt.hpp>
#include "Core/EventListener.h"
#include "Util/LuaUtil.h"

class Entity
{
public:
	Entity(entt::entity id, entt::registry& registry, EventPublisher& eventPub, std::string name = "", std::string templateTag = "")
		: mRegistry(registry), mEventPublisher(eventPub), mId(id), mName(std::move(name)), mTemplateTag(std::move(templateTag))
	{
	}

	const entt::entity& GetId() const { return mId; }
	const std::string& GetName() const { return mName; }
	const std::string& GetTemplateTag() const { return mTemplateTag; }
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
	Component& AddComponent(Args&&... args)
	{
		mRegistry.emplace<Component>(mId, std::forward<Args>(args)...);
		mEventPublisher.Notify("on_construct", std::type_index(typeid(Component)), mId);
		return mRegistry.get<Component>(mId);
	}

	template <typename Component>
	void RemoveComponent() const
	{
		mRegistry.remove<Component>(mId);
		mEventPublisher.Notify("on_destroy", std::type_index(typeid(Component)), mId);
	}

	template <typename... Components>
	bool HasComponent() const
	{
		return mRegistry.all_of<Components...>(mId);
	}

	bool IsValid() const { return mRegistry.valid(mId); }

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
	std::string mTemplateTag;
};
