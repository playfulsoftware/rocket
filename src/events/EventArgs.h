// EventArgs.h
// Copyright 2012 Playful Software

#ifndef ROCKET_EVENTS_EVENTARGS_H
#define ROCKET_EVENTS_EVENTARGS_H

namespace Rocket {
namespace Events {

class EventArgs { 
    public:
        virtual void* getEventData() const = 0;
};

} // Events
} // Rocket

#endif // ROCKET_EVENTS_EVENTARGS_H
