// EventHandler.h
// Copyright 2012, Playful Software

#ifndef ROCKET_EVENTS_EVENTHANDLER_H
#define ROCKET_EVENTS_EVENTHANDLER_H

#include "fastdelegate.h"

namespace Rocket {
namespace Events {

typedef fastdelegate::FastDelegate<void (void *)> EventHandler;

} // Events
} // Rocket

#endif // ROCKET_EVENTS_EVENTHANDLER_H
