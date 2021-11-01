#pragma once

class EntityContext {
public:
	EntityContext(EnTTRegistry& registry, EntityId id);
	~EntityContext();

	template<typename T>
	T& GetComponent() {
		return mRegistry.get<T>(mId);
	}

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args) {
		if (HasComponent<T>()) {
			assert("Entity already has this component");
		}
		return mRegistry.emplace<T>(mId, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	T& AddOrGetComponent(Args&&... args) {
		return mRegistry.emplace_or_replace<T>(mId, std::forward<Args>(args)...);

	}

	template<typename Component>
	void RemoveComponent() {
		mRegistry.remove<Component>(mId);
	}

	template<typename T>
	bool HasComponent()
	{
		return mRegistry.all_of<T>(mId);
	}

	EntityId getId();

private:
	friend class EntityRegistry;

	EnTTRegistry& mRegistry;
	EntityId mId;
};
