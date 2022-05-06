// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/Lua/GKLuaScript.h"

// Unreal Engine
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#ifdef WITH_LUA
extern "C"
{
#define LUA_COMPAT_APIINTCASTS 1
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#endif

void UGKLuaScript::Compile() {
#ifdef WITH_LUA
	bool bDebug = false;

	lua_State* LuaState = lua_open();

	FString Code;
	FFileHelper::LoadFileToString(Code, *GetScriptPath());

	// load a null terminated string
	luaL_loadstring(LuaState, TCHAR_TO_ANSI(*Code));

	auto Writer = [](lua_State* l, const void* p, size_t size, void* userdata) -> int {
		TArray<uint8>* Buffer = (TArray<uint8>*)userdata;
		Buffer->Append((uint8*)p, size);
	};

	lua_dump(LuaState, Writer, (void*)(&ByteCote), !bDebug);
#endif
}
