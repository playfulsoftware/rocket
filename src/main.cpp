#include <iostream>


#include <SDL/SDL.h>

#include <private/LuaVM.hpp>

#include <private/SimpleEventListener.hpp>
#include <private/SimpleEventSource.hpp>

class SDLEngineLoop : public EventSource {

    private:
        bool shouldRun;
        SimpleEventSource evtSrc;

    public:
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

            emit("EngineInit");

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

            emit("EngineShutdown");

            return 0;
        }

        virtual bool addListener(EventListener* l) {
            evtSrc.addListener(l);
        }

        virtual bool removeListener(EventListener *l) {
            evtSrc.removeListener(l);
        }

        virtual void emit(const char* e) {
            evtSrc.emit(e);
        }
};

int main(int argc, char** argv) {

    SDLEngineLoop main;
    LuaVM vm;

    main.addListener(&vm);

    vm.runScriptFile("test.lua");

    return main.start();
}

