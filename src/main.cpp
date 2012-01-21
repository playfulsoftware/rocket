#include <iostream>

#include <lua.hpp>

#include <private/SimpleEvent.hpp>
#include <private/SimpleEventListener.hpp>
#include <private/SimpleEventSource.hpp>

int main(int argc, char** argv) {
    SimpleEvent e("MyEvent");
    SimpleEventListener l;
    SimpleEventSource s;

    s.addListener(&l);
    s.emit(&e);

    std::cout << "creating lua vm" << std::endl;
    lua_State* state = luaL_newstate();
    std::cout << "opening std libraries" << std::endl;
    luaL_openlibs(state);
    std::cout << "running lua test script." << std::endl;
    luaL_dostring(state, "print(\"Hello, PSEngine!\")");
    std::cout << "closing lua vm" << std::endl;
    lua_close(state);

    return 0;
}

