#include <iostream>

#include <lua.hpp>

#include <private/SimpleEvent.hpp>
#include <private/SimpleEventListener.hpp>
#include <private/SimpleEventSource.hpp>

class LuaVM {

    private:
        lua_State* vmState;

    public:
        LuaVM() {
            vmState = luaL_newstate();
            luaL_openlibs(vmState);
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
};

int main(int argc, char** argv) {
    SimpleEvent e("MyEvent");
    SimpleEventListener l;
    SimpleEventSource s;

    LuaVM vm;

    s.addListener(&l);
    s.emit(&e);

    vm.runScriptString("print(\"Hello!\")");

    return 0;
}

