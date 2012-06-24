// SDLPlatform.h
// Copyright 2012, Playful Software

#ifndef ROCKET_PLATFORM_SDL_SDLPLATFORM_H
#define ROCKET_PLATFORM_SDL_SDLPLATFORM_H

#include <deque>
#include <map>
#include <string>

#include "events/EventArgs.h"
#include "events/EventEmitter.h"

namespace Rocket {
namespace Platform {

class SDLEventArgs : public Rocket::Events::EventArgs {
    public:
        virtual void* getEventData() const { return NULL; }
};

class SDLPlatform : public Rocket::Events::EventEmitter {

    public:

        SDLPlatform();
        ~SDLPlatform();

        // implement the EventEmitter interface
        virtual void on(const char* eventName, const Rocket::Events::EventHandler& e);

        void createWindow(int width, int height);

        int run();

    private:
        // declare dummy copy-operators to make the class non-copyable.
        SDLPlatform(const SDLPlatform& other); 
        SDLPlatform& operator=(const SDLPlatform& other); 

        void emit(const char* name, const Rocket::Events::EventArgs& e);

        bool isRunning;

        std::map< std::string, std::deque<Rocket::Events::EventHandler> > events;


};

} // Platform
} // Rocket

#endif // ROCKET_PLATFORM_SDL_SDLPLATFORM_H

