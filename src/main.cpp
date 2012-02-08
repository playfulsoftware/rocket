#include <iostream>

#include <vector>

#include <lua.hpp>

#include <private/LuaVM.hpp>

#include <private/SimpleEvent.hpp>
#include <private/SimpleEventListener.hpp>
#include <private/SimpleEventSource.hpp>



int main(int argc, char** argv) {
    SimpleEvent e("MyEvent");
    SimpleEventListener l;
    SimpleEventSource s;

    LuaVM vm;

    s.addListener(&l);
    s.addListener(&vm);

    vm.addListener(&l);


    vm.runScriptFile("test.lua");

    s.emit(&e);

    return 0;
}

