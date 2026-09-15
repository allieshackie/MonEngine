#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/ScriptComponent.h"
#include "Util/LuaUtil.h"

#include "LuaSystem.h"

//#define PROFILE_LUA

LuaSystem::LuaSystem(EventPublisher& eventPublisher)
{
	mLuaContext = std::make_unique<LuaContext>();

	lua_newtable(mLuaContext->GetState());
	lua_setglobal(mLuaContext->GetState(), LuaUtil::mGameData.c_str());

	eventPublisher.AddWorldCreatedListener(
		[this](std::weak_ptr<World> world) {
			_ReleaseAllScripts();

			if (const auto worldShared = world.lock())
			{
				World* worldPtr = worldShared.get();
				EntityEventFunc func = [this, worldPtr](entt::entity entityId)
				{
					if (Entity* entity = worldPtr->GetEntityForId(entityId))
					{
						ScriptComponent& script = entity->GetComponent<ScriptComponent>();
						script.mLuaTableRef = LuaUtil::CreateLuaTable<Entity>(mLuaContext->GetState(), entity);
						mScriptRefs[entityId] = script.mLuaTableRef;
						mLuaContext->ExecuteWithInstance(script.mPath.c_str(), script.mLuaTableRef);
						mLuaContext->Initialize(script.mLuaTableRef);
					}
				};
				worldShared->ConnectOnConstruct<ScriptComponent>(func);

				EntityEventFunc onDestroy = [this](entt::entity entityId)
				{
					_ReleaseScript(entityId);
				};
				worldShared->ConnectOnDestroy<ScriptComponent>(onDestroy);

				PhysicsEventFunc onEnterFunc = [this, worldPtr](entt::entity entityA, entt::entity entityB)
				{
					if (Entity* entity = worldPtr->GetEntityForId(entityA))
					{
						const ScriptComponent& script = entity->GetComponent<ScriptComponent>();
						mLuaContext->CallMethod(script.mLuaTableRef, "OnTriggerEnter");
					}
				};
				worldShared->ConnectOnPhysicsEvent(PhysicsEventType::TriggerEnter, onEnterFunc);
				PhysicsEventFunc onExitFunc = [this, worldPtr](entt::entity entityA, entt::entity entityB)
				{
					if (Entity* entity = worldPtr->GetEntityForId(entityA))
					{
						const ScriptComponent& script = entity->GetComponent<ScriptComponent>();
						mLuaContext->CallMethod(script.mLuaTableRef, "OnTriggerExit");
					}
				};
				worldShared->ConnectOnPhysicsEvent(PhysicsEventType::TriggerExit, onExitFunc);
				
			}
			mWorld = world;
		}
	);
}

void LuaSystem::_ReleaseScript(entt::entity entityId)
{
	const auto it = mScriptRefs.find(entityId);
	if (it != mScriptRefs.end())
	{
		luaL_unref(mLuaContext->GetState(), LUA_REGISTRYINDEX, it->second);
		mScriptRefs.erase(it);
	}
}

void LuaSystem::_ReleaseAllScripts()
{
	for (const auto& [entityId, tableRef] : mScriptRefs)
	{
		luaL_unref(mLuaContext->GetState(), LUA_REGISTRYINDEX, tableRef);
	}
	mScriptRefs.clear();
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
		view.each([this](auto& script)
		{
			if (!script.mIsTrigger)
			{
				mLuaContext->Update(script.mLuaTableRef);
			}	
		});
	}
}
