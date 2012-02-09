#include <iostream>

#include <vector>

#include <lua.hpp>

#include <private/LuaVM.hpp>

#include <private/SimpleEventListener.hpp>
#include <private/SimpleEventSource.hpp>


class EventLoop : public EventSource {
};

int main(int argc, char** argv) {
    SimpleEventListener l;
    SimpleEventSource s;

    LuaVM vm;

    s.addListener(&l);
    s.addListener(&vm);

    vm.addListener(&l);


    vm.runScriptFile("test.lua");

    s.emit("MyEvent");

    return 0;
}

