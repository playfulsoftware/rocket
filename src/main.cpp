#include <iostream>

#include <vector>

#include <lua.hpp>

#include <public/EventListener.hpp>
#include <public/EventSource.hpp>

#include <private/SimpleEvent.hpp>
#include <private/SimpleEventListener.hpp>
#include <private/SimpleEventSource.hpp>


class LuaVM : public EventListener, public EventSource {

    private:
        lua_State* vmState;
        SimpleEventSource eventSrc;

        static LuaVM* getVMSelf(lua_State* L) {
            lua_getfield(L, LUA_REGISTRYINDEX, "vmState");
            LuaVM* self = (LuaVM*) lua_touserdata(L, -1);
            lua_pop(L, 1);
            return self;
        }

        static int luaTrace(lua_State* L) {
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

        static int luaRegisterEvent(lua_State* L) {
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
                }
            }
            return 0;
        }

        static int luaEmitEvent(lua_State* L) {
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
                    SimpleEvent e(luaL_tolstring, nargs, NULL);
                    self->emit(&e);
                }
            }
            return 0;
        }

    public:
        LuaVM() {

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

        ~LuaVM() {
            lua_close(vmState);
        }

        void runScriptFile(const char* file) {
            if (luaL_dofile(vmState, file) != 0) {
                std::cerr << "an error occurred while executing the script file: " << file << std::endl;
            }
        }

        void runScriptString(const char* buffer) {
            if (luaL_dostring(vmState, buffer) != 0) {
                std::cerr << "an error occurred while executing the script buffer" << std::endl;
            }

        }

        virtual bool addListener(EventListener* listener) {
            eventSrc.addListener(listener);
        }

        virtual bool removeListener(EventListener* listener) {
            eventSrc.removeListener(listener);
        }

        virtual void emit(const Event* e) {
            // dispatch event to lua listeners.
            // emitLuaEvent(e);
            // dispatch event to c listeners
            eventSrc.emit(e);
        }
};

int main(int argc, char** argv) {
    SimpleEvent e("MyEvent");
    SimpleEventListener l;
    SimpleEventSource s;

    LuaVM vm;

    s.addListener(&l);
    s.emit(&e);

    vm.runScriptFile("test.lua");

    return 0;
}

