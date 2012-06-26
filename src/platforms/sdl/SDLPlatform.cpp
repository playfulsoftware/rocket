// SDLPlatform.cpp
// Copyright 2012 Playful Software

#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>

#include "platforms/sdl/SDLPlatform.h"

namespace Rocket {
namespace Platform {

SDLPlatform::SDLPlatform() :
  isRunning(false) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        exit(1);
    } else {
    }
}

SDLPlatform::~SDLPlatform() {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDLPlatform::createWindow(int width, int height) {

    // TODO:: Move the OpenGL specific code out into more appropriate place.
    // define the GL settings
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // initialize the window
    window = SDL_CreateWindow("Rocket Engine Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        // bail
        exit(1);
    }

    context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1);
}

void SDLPlatform::emit(const char* name, const Rocket::Events::EventArgs &e) {
    if (events.find(name) != events.end()) {
        std::deque<Rocket::Events::EventHandler>::iterator iter = events[name].begin();
        for (iter; iter != events[name].end(); ++iter) {
            (*iter)(e);
        }
    }
}


void SDLPlatform::on(const char* eventName, const Rocket::Events::EventHandler& handler) {
            if (events.find(eventName) == events.end()) {
                std::deque<Rocket::Events::EventHandler> queue;
                events[eventName] = queue;
            }
            events[eventName].push_back(handler);
        }

int SDLPlatform::run() {
    SDLEventArgs dummy;
    emit("MainLoopStart", dummy);

    isRunning = true;

    SDL_Event evt;

    while (isRunning) {
        //handle SDL events. 
        while (SDL_PollEvent(&evt)) {
            switch (evt.type) {
                case SDL_QUIT:
                    isRunning = false;
                    emit("MainLoopStop", dummy);
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}

} // Platform
} // Rocket
