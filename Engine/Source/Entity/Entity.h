#pragma once
#include "Core/Scene.h"
#include <entt/entity/entity.hpp>

struct MonScene;

class Entity
{
public:
	Entity(entt::entity id) : mId(id)
	{
	}

	template <typename Component>
	Component& GetComponent()
	{
		return mScene->mRegistry.get<Component>(mId);
	}

	template <typename Component>
	Component* TryGetComponent()
	{
		return mScene->mRegistry.try_get<Component>(mId);
	}

	template <typename Component, typename... Args>
	void AddComponentWithArgs(Args&&... args)
	{
		mScene->mRegistry.emplace<Component>(mId, std::forward<Args>(args)...);
	}

	template <typename Component>
	void AddComponent(Component component)
	{
		mScene->mRegistry.emplace<Component>(mId, component);
	}

	template <typename Component, typename... Args>
	Component& AddOrGetComponent(Args&&... args)
	{
		return mScene->mRegistry.emplace_or_replace<Component>(mId, std::forward<Args>(args)...);
	}

	template <typename Component>
	void RemoveComponent() const
	{
		mScene->mRegistry.remove<Component>(mId);
	}

	template <typename... Components>
	bool HasComponent() const
	{
		return mScene->mRegistry.all_of<Components>(mId);
	}

private:
	std::shared_ptr<MonScene> mScene = nullptr;

	entt::entity mId{entt::null};
	const char* mName = "";
};
