#include <iostream>

#include "LuaVM.h"

LuaVM* LuaVM::getVMSelf(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "vmState");
    LuaVM* self = (LuaVM*) lua_touserdata(L, -1);
    lua_pop(L, 1);
    return self;
}

int LuaVM::luaTrace(lua_State* L) {
    int nargs = lua_gettop(L);

    if (nargs == 0) {
        std::cerr << "lua:" << std::endl;
        return 0;
    } else if (nargs == 1) {
        std::cerr << "lua: " << luaL_tolstring(L, nargs, NULL) << std::endl;
        lua_pop(L, 1);
        return 1;
    } else {
        lua_pushstring(L, "Too many arguments");
        lua_error(L);
    }
}


LuaVM::LuaVM() {

    vmState = luaL_newstate();
    //luaL_openlibs(vmState);

    // store a reference to the vm in the lua registry.
    lua_pushlightuserdata(vmState, this);
    lua_setfield(vmState, LUA_REGISTRYINDEX, "vmState");

    // add the trace() command.
    lua_pushcfunction(vmState, luaTrace);
    lua_setglobal(vmState, "trace");
}

LuaVM::~LuaVM() {
    lua_close(vmState);
}

bool LuaVM::runScriptFile(const char* file) {
    return luaL_dofile(vmState, file) == 0; 
}

bool LuaVM::runScriptString(const char* buffer) {
    return luaL_dostring(vmState, buffer) == 0;
}
