#include "LuaSystem.h"

//#define PROFILE_LUA

LuaSystem::LuaSystem()
{
	mLuaContext = std::make_unique<LuaContext>();
}

void LuaSystem::LoadScript(const char* scriptFile)
{
	mLuaContext->Execute(scriptFile);
}

void LuaSystem::QueueClose()
{
	mQueueClose = true;
}

void LuaSystem::CloseAllScripts()
{
	mQueueClose = false;
}

bool LuaSystem::QueueCloseScripts() const
{
	return mQueueClose;
}
