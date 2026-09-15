#pragma once
#include "Core/ISystem.h"

#include "LuaContext.h"

class EventPublisher;

class LuaSystem : public ISystem
{
public:
	LuaSystem(EventPublisher& eventPublisher);

	lua_State* GetState() const;

	void LoadScript(const char* scriptFile) const;
	void Update(float dt) override;

private:
	void _ReleaseScript(entt::entity entityId);
	void _ReleaseAllScripts();

	std::unique_ptr<LuaContext> mLuaContext;
	std::weak_ptr<World> mWorld;
	std::unordered_map<entt::entity, int> mScriptRefs;
};
