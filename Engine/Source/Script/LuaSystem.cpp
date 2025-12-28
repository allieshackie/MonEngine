#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/ScriptComponent.h"
#include "Util/LuaUtil.h"

#include "LuaSystem.h"

//#define PROFILE_LUA

LuaSystem::LuaSystem(std::weak_ptr<World> world) : mWorld(world)
{
	mLuaContext = std::make_unique<LuaContext>();

	lua_newtable(mLuaContext->GetState());
	lua_setglobal(mLuaContext->GetState(), LuaUtil::mGameData.c_str());

	if (const auto worldPtr = mWorld.lock())
	{
		EventFunc func = [this](Entity* entity)
		{
			ScriptComponent& script = entity->GetComponent<ScriptComponent>();
			script.mLuaTableRef = LuaUtil::CreateLuaTable<Entity>(mLuaContext->GetState(), entity);
			mLuaContext->ExecuteWithInstance(script.mPath.c_str(), script.mLuaTableRef);
			mLuaContext->Initialize(script.mLuaTableRef);
		};
		worldPtr->ConnectOnConstruct<ScriptComponent>(func);

		EventFunc onDestroy = [this](Entity* entity)
		{
			const ScriptComponent& script = entity->GetComponent<ScriptComponent>();
			luaL_unref(mLuaContext->GetState(), LUA_REGISTRYINDEX, script.mLuaTableRef);
		};
		worldPtr->ConnectOnDestroy<ScriptComponent>(func);
	}
}

lua_State* LuaSystem::GetState() const
{
	return mLuaContext->GetState();
}

void LuaSystem::LoadScript(const char* scriptFile) const
{
	mLuaContext->Execute(scriptFile);
}

void LuaSystem::Update(float dt)
{
	if (const auto world = mWorld.lock())
	{
		const auto view = world->GetRegistry().view<ScriptComponent>();
		view.each([=](auto& script)
		{
			mLuaContext->Update(script.mLuaTableRef);
		});
	}
}
