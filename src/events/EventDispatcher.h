#ifndef ROCKET_EVENTDISPATCHER
#define ROCKET_EVENTDISPATCHER

#ifdef USE_KQUEUE_EVENTS
#include "platform/kqueue/KQueueEventDispatcher.h"
namespace Rocket {
    typedef KQueueEventDispatcher EventDispatcher;
} // Rocket
#endif

#endif // ROCKET_EVENTDISPATCHER
