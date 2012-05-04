#include <deque>
#include <iostream>
#include <string>

#include <SDL/SDL.h>

#include "FastDelegate.h"
#include "LuaVM.h"

using namespace fastdelegate;

class EventArgs { 
    public:
        virtual const char* getEventName() = 0;
        virtual void* getEventData() = 0;
};

typedef FastDelegate<void (const EventArgs &)> EventHandler;

// We differentiate between arguments to allow for the binding of subclasses
// to an ancestor's member function pointer.
template <class X, class Y>
EventHandler makeHandler(X* klass, void (Y::*func_ptr)(const EventArgs&)) {
    return EventHandler(klass, func_ptr);
}

class Event {
    public:
        void on(const EventHandler& handler) {
            handlers.push_back(handler);
        }

        void operator()(const EventArgs &e) {
            std::deque<EventHandler>::iterator iter = handlers.begin();
            for (iter; iter != handlers.end(); ++iter) {
                (*iter)(e);
            }
        }

    private:
        std::deque<EventHandler> handlers;
};

class SDLEventArgs : public EventArgs {
    public:
        SDLEventArgs(std::string evt_name) { name = evt_name; }

        virtual const char* getEventName() { return name.c_str(); }
        virtual void* getEventData() { return NULL; }

    private:
        std::string name;
};

class SDLEngineLoop{

    private:
        bool shouldRun;

    public:

        Event engineEvents;

        SDLEngineLoop() : shouldRun(true) {
            if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
                std::cerr << "fatal error: unable to initialize SDL" << std::endl;
                exit(1);
            } else {
                std::cerr << "SDL initialized." << std::endl;
            }
        }

        ~SDLEngineLoop() { 
            std::cerr << "shutting down SDL." << std::endl;
            SDL_Quit(); 
        }

        int start() {
            std::cerr << "sending start loop message" << std::endl;
            SDLEventArgs start("SDLLoopStart");
            engineEvents(start);
            std::cerr << "message sent" << std::endl;

            SDL_Event evt;

            while (shouldRun) {
                //handle SDL events. 
                while (SDL_PollEvent(&evt)) {
                    switch (evt.type) {
                        case SDL_QUIT:
                            shouldRun = false;
                            break;
                        default:
                            break;
                    }
                }
            }

            return 0;
        }

};

void test(const EventArgs& e) {
    std::cerr << "in event handler func" << std::endl;
}

class TestClass {
    public:
        void func(const EventArgs& e) {
            std::cerr << "in member event handler func" << std::endl;
        }
};

int main(int argc, char** argv) {

    SDLEngineLoop main;

    TestClass t;

    main.engineEvents.on(EventHandler(test));
    main.engineEvents.on(EventHandler(&t, &TestClass::func));
    //LuaVM vm;

    //if (!vm.runScriptFile("test.lua")) {
    //    std::cerr << "an error occurred while loading the lua startup script" << std::endl;
    //    return 1;
    //}


    return main.start();
}

