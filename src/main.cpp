#include <iostream>
#include <string>

#include "platforms/sdl/SDLPlatform.h"
#include "scripting/LuaInstance.h"


int main(int argc, char** argv) {

    Rocket::Platform::SDLPlatform sdlPlatform;

    sdlPlatform.createWindow(640, 480);

    return sdlPlatform.run();

}

