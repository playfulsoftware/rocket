#include <iostream>


#include <SDL/SDL.h>

#include "LuaVM.hpp"

class SDLEngineLoop{

    private:
        bool shouldRun;

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

int main(int argc, char** argv) {

    SDLEngineLoop main;
    LuaVM vm;

    if (!vm.runScriptFile("test.lua")) {
        std::cerr << "an error occurred while loading the lua startup script" << std::endl;
        //luaL_error(vmState, "lua-fatal");
        return 1;
    }


    return main.start();
}

