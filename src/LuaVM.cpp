#include <private/LuaVM.hpp>

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

int LuaVM::luaRegisterEvent(lua_State* L) {
    int nargs = lua_gettop(L);

    // validate imput
    if (nargs < 2) {
        lua_pushstring(L, "Too few arguments");
        lua_error(L);
    } else if (nargs > 2) {
        lua_pushstring(L, "Too many arguments");
        lua_error(L);
    } else {
        if (! lua_isstring(L, 1) ) { 
            lua_pushstring(L, "Argument 1 must be a string");
            lua_error(L);
        } else if (! lua_isfunction(L, 2)) {
            lua_pushstring(L, "Argument 2 must be a function");
        } else {
            // check to see if the event name already has a function table in the lua state.
            // if so, simply append the function to the end of that table.
            // if not, create a new table with the function as its only member.

            // get the root cb table.
            lua_getfield(L, LUA_REGISTRYINDEX, "event_cbs");

            // move the string parameter to the top of the stack
            lua_pushnil(L);
            lua_copy(L, 1, -1);

            // swap the copy of the string for the function list it points to.
            lua_gettable(L, -2);

            if ( lua_isnil(L, -1) ) {
                //std::cerr << "creating new callback table." << std::endl;
                // create new table
                lua_pop(L, 1);
                lua_newtable(L);

                lua_pushinteger(L, 1);

                // copy the function to the top of the stack.
                lua_pushnil(L);
                lua_copy(L, 2, -1);

                // assign the function to index 1.
                lua_settable(L, -3);

                // assign the new table to the function table
                lua_setfield(L, -2, lua_tolstring(L, 1, NULL));

                // pop the function table reference off the stack.
                lua_pop(L, 1);


            } else {
                //std::cerr << "appending callback to existing table." << std::endl;
                // get the length of the current table.
                lua_len(L, -1);

                // push the integer 1 on to the stack.
                lua_pushinteger(L, 1);

                // add 1 to the existing table lengh;
                lua_arith(L, LUA_OPADD);

                // copy the function to the top of the stack.
                lua_pushnil(L);
                lua_copy(L, 2, -1);

                // assign the function to the newest index.
                lua_settable(L, -3);

                // pop the table reference off the stack.
                lua_pop(L, 1);
            }
        }
    }
    return 0;
}

int LuaVM::luaEmitEvent(lua_State* L) {
    int nargs = lua_gettop(L);

    // validate input
    if (nargs < 1) {
        lua_pushstring(L, "Too few arguments");
        lua_error(L);
    } else if (nargs > 1) {
        lua_pushstring(L, "Too many arguments");
    } else {
        if (! lua_isstring(L, 1)) {
            lua_pushstring(L, "Event name must be a string.");
            lua_error(L);
        } else {
            // get a reference to the vm
            LuaVM* self = getVMSelf(L);
            self->emit(luaL_tolstring(L, nargs, NULL));
            lua_pop(L, 1);
        }
    }
    return 0;
}

void LuaVM::emitLuaEvent(const char* e) {
    int ret = 0;
    // get the master event callback table.
    lua_getfield(vmState, LUA_REGISTRYINDEX, "event_cbs");
    // get our specific event callback sub-table.
    lua_getfield(vmState, -1, e);

    if (!lua_isnil(vmState, -1)) {
        //std::cerr << "found callback table" << std::endl;
        // iterate the table, and call each function with the event name.
        lua_pushnil(vmState);
        while (lua_next(vmState, -2) != 0) {
            lua_pushstring(vmState, e);
            ret = lua_pcall(vmState, 1, 0, 0);
            if (ret != LUA_OK) {
                std::cerr << "an error occurred while calling a lua callback function: " << lua_tolstring(vmState, -1, NULL) << std::endl;
                lua_error(vmState);
            }
        }

    } else {
        //std::cerr << "unable to find callback table." << std::endl;
    }
    // pop the table references off the stack.
    lua_pop(vmState, 2);

}

LuaVM::LuaVM() {

    vmState = luaL_newstate();
    //luaL_openlibs(vmState);

    // store a reference to the vm in the lua registry.
    lua_pushlightuserdata(vmState, this);
    lua_setfield(vmState, LUA_REGISTRYINDEX, "vmState");

    // create a table to store the lua event callbacks.
    lua_newtable(vmState);
    lua_setfield(vmState, LUA_REGISTRYINDEX, "event_cbs");

    // add the trace() command.
    lua_pushcfunction(vmState, luaTrace);
    lua_setglobal(vmState, "trace");

    // add the on_event() command.
    lua_pushcfunction(vmState, luaRegisterEvent);
    lua_setglobal(vmState, "on_event");

    // add the emit() command.
    lua_pushcfunction(vmState, luaEmitEvent);
    lua_setglobal(vmState, "emit");
}

LuaVM::~LuaVM() {
    lua_close(vmState);
}

void LuaVM::runScriptFile(const char* file) {
    if (luaL_dofile(vmState, file) != 0) {
        std::cerr << "an error occurred while executing the script file: " << file << std::endl;
    }
}

void LuaVM::runScriptString(const char* buffer) {
    if (luaL_dostring(vmState, buffer) != 0) {
        std::cerr << "an error occurred while executing the script buffer" << std::endl;
    }

}

bool LuaVM::addListener(EventListener* listener) {
    eventSrc.addListener(listener);
}

bool LuaVM::removeListener(EventListener* listener) {
    eventSrc.removeListener(listener);
}

void LuaVM::emit(const char* e) {
    // dispatch event to lua listeners.
    emitLuaEvent(e);

    // dispatch event to c listeners
    eventSrc.emit(e);
}

void LuaVM::onEvent(const EventSource* src, const char* e) {
    emitLuaEvent(e);
}

