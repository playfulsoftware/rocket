
#include <iostream>

#include "LuaVM.h"

static int luaNoOp(lua_State* L) {
    return 0;
}

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

int LuaVM::luaRequire(lua_State* L) {
    return 0;
}

int LuaVM::luaModule(lua_State* L) {
    return 0;
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

    // add the require() function.
    lua_pushcfunction(vmState, luaRequire);
    lua_setglobal(vmState, "require");

    // add the module() function.
    lua_pushcfunction(vmState, luaModule);
    lua_setglobal(vmState, "module");
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

bool LuaVM::runStartupScript(const char* file) {
    int top = lua_gettop(vmState);
    bool ret = luaL_dofile(vmState, file) == 0;
    if (ret) { 
        // get the number of return values pushed onto the stack.
        int delta = lua_gettop(vmState) - top;
        if (delta == 0) {
            std::cerr << "no return value from lua config script detected. no config values defined." << std::endl;
            return false;
        } else {
            // we only care about the first result returned, and then only if it's a table.
            if (!lua_istable(vmState, top + 1)) {
                std::cerr << "returned config object is not a table. no config values defined" << std::endl;
                return false;
            } else {
                // set the first result to the top of the stack.
                lua_settop(vmState, top + 1);
                // create a new table to serve as a metatable.
                lua_newtable(vmState);
                // push the no-op function onto the stack.
                lua_pushcfunction(vmState, luaNoOp);
                // set the no-op function as the __newindex method. this effectively makes the table read-only.
                lua_setfield(vmState, -2, "__newindex");
                lua_setmetatable(vmState, -2);
                // set the returned table to be the global "config".
                lua_setglobal(vmState, "config");
            }
        }
    } else {
        const char* msg = lua_tostring(vmState, -1);
        msg = (msg == NULL) ? "Unknown error" : msg;
        std::cerr << "lua startup error: " << msg << std::endl;
    }
    return ret;
}

std::string LuaVM::getConfigString(const char* name) {
    lua_getglobal(vmState, "config");
    lua_getfield(vmState, -1, name);
    return lua_tostring(vmState, -1);
}

double LuaVM::getConfigNumber(const char* name) {
    lua_getglobal(vmState, "config");
    lua_getfield(vmState, -1, name);
    return lua_tonumber(vmState, -1);
}
