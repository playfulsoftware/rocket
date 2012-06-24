#include <iostream>
#include <string>

#include <SDL/SDL.h> // need to include the SDL header so the main gets remapped correctly.
#include "platforms/sdl/SDLPlatform.h"
#include "scripting/LuaInstance.h"


int main(int argc, char** argv) {

    Rocket::Platform::SDLPlatform sdlPlatform;

    sdlPlatform.createWindow(640, 480);

    return sdlPlatform.run();

}

