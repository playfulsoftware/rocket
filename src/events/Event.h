#ifndef ROCKET_EVENTS_EVENT_H
#define ROCKET_EVENTS_EVENT_H

namespace Rocket {
namespace Events {

typedef enum {
    MESSAGE_EVENT
} EventType;

typedef struct _Event {
    EventType type;
    uint32_t size;
    void* data;
} Event;

} // Events
} // Rocket

#endif // ROCKET_EVENTS_EVENT_H
