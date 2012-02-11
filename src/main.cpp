#include <iostream>


#include <SDL/SDL.h>

#include <private/LuaVM.hpp>

#include <private/SimpleEventListener.hpp>
#include <private/SimpleEventSource.hpp>

class SDLEngineLoop : public EventSource {
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

