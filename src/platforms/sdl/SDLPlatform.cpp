// SDLPlatform.cpp
// Copyright 2012 Playful Software

#include <SDL/SDL.h>

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
    SDL_Quit();
}

void SDLPlatform::createWindow(int width, int height) {

    // TODO:: Move the OpenGL specific code out into more appropriate place.
    // define the GL settings
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    int flags = SDL_OPENGL;

    // initialize the window
    const SDL_VideoInfo* info = SDL_GetVideoInfo();

    if (SDL_SetVideoMode(width, height, info->vfmt->BitsPerPixel, flags) == 0)
    {
        // bail
        exit(1);
    }
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
