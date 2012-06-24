
#include <string.h>
#include <iostream>

#include "scripting/LuaInstance.h"

static int luaNoOp(lua_State* L) {
    return 0;
}

namespace Rocket {
namespace Scripting {

// public methods

LuaInstance::LuaInstance() {

    vmState = luaL_newstate();
    //luaL_openlibs(vmState);

    // store a reference to the vm in the lua registry.
    lua_pushlightuserdata(vmState, this);
    lua_setfield(vmState, LUA_REGISTRYINDEX, "vmState");

    // add the trace() command.
    lua_pushcfunction(vmState, luaTrace);
    lua_setglobal(vmState, "trace");

    // add the require() function.
    lua_pushcfunction(vmState, luaRequire);
    lua_setglobal(vmState, "require");

    // add the module() function.
    lua_pushcfunction(vmState, luaModule);
    lua_setglobal(vmState, "module");
}

LuaInstance::~LuaInstance() {
    lua_close(vmState);
}


bool LuaInstance::runBuffer(const char* buffer) {
    return luaL_dostring(vmState, buffer) == 0;
}

bool LuaInstance::getGlobalValue(const char* name, int* value) {

    lua_getglobal(vmState, name);
    if (lua_isnil(vmState, -1)) {
        // symbol doesn't exist
        return false;
    } else {
        *value = (int)lua_tointeger(vmState, -1);
        return true;
    }
}

bool LuaInstance::getGlobalValue(const char* name, long* value) {

    lua_getglobal(vmState, name);
    if (lua_isnil(vmState, -1)) {
        // symbol doesn't exist
        return false;
    } else {
        *value = (long)lua_tointeger(vmState, -1);
        return true;
    }
}

bool LuaInstance::getGlobalValue(const char* name, float* value) {

    lua_getglobal(vmState, name);
    if (lua_isnil(vmState, -1)) {
        // symbol doesn't exist
        return false;
    } else {
        *value = (float)lua_tonumber(vmState, -1);
        return true;
    }
}

bool LuaInstance::getGlobalValue(const char* name, double* value) {

    lua_getglobal(vmState, name);
    if (lua_isnil(vmState, -1)) {
        // symbol doesn't exist
        return false;
    } else {
        *value = (double)lua_tonumber(vmState, -1);
        return true;
    }
}

bool LuaInstance::getGlobalValue(const char* name, bool* value) {

    lua_getglobal(vmState, name);
    if (lua_isnil(vmState, -1)) {
        // symbol doesn't exist
        return false;
    } else {
        *value = (bool)lua_toboolean(vmState, -1);
        return true;
    }
}

bool LuaInstance::getGlobalValue(const char* name, char** value) {

    lua_getglobal(vmState, name);
    if (lua_isnil(vmState, -1)) {
        // symbol doesn't exist
        return false;
    } else {
        // TODO:: Fix up this lazy (and probably wrong) string handling.
        *value = strdup(lua_tolstring(vmState, -1, NULL));
        return true;
    }
}

void LuaInstance::setGlobalValue(const char* name, int value) {
    lua_pushinteger(vmState, value);
    lua_setglobal(vmState, name);
}

void LuaInstance::setGlobalValue(const char* name, long value) {
    lua_pushinteger(vmState, value);
    lua_setglobal(vmState, name);
}

void LuaInstance::setGlobalValue(const char* name, float value) {
    lua_pushnumber(vmState, value);
    lua_setglobal(vmState, name);
}

void LuaInstance::setGlobalValue(const char* name, double value) {
    lua_pushnumber(vmState, value);
    lua_setglobal(vmState, name);
}

void LuaInstance::setGlobalValue(const char* name, bool value) {
    lua_pushboolean(vmState, value);
    lua_setglobal(vmState, name);
}

void LuaInstance::setGlobalValue(const char* name, const char* value) {
    lua_pushlstring(vmState, value, strlen(value));
    lua_setglobal(vmState, name);
}

void LuaInstance::DeleteGlobal(const char* name) {
    lua_pushnil(vmState);
    lua_setglobal(vmState, name);
}


// private methods

LuaInstance* LuaInstance::getVMSelf(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "vmState");
    LuaInstance* self = (LuaInstance*) lua_touserdata(L, -1);
    lua_pop(L, 1);
    return self;
}

// private static methods, aka lua hooks.

int LuaInstance::luaTrace(lua_State* L) {
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

int LuaInstance::luaRequire(lua_State* L) {
    return 0;
}

int LuaInstance::luaModule(lua_State* L) {
    return 0;
}


} // Scripting
} // Rocket
