// EventEmitter.h
// Copyright 2012 Playful Software

#ifndef ROCKET_EVENTS_EVENTEMITTER_H
#define ROCKET_EVENTS_EVENTEMITTER_H

#include "events/EventHandler.h"

namespace Rocket {
namespace Events {

class EventEmitter {
    public:
        virtual void on(const char* eventName, const EventHandler& handler) = 0;
};

} // Events
} // Rocket

#endif // ROCKET_EVENTS_EVENTEMITTER_H
