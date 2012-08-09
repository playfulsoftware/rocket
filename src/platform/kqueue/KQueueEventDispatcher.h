#ifndef ROCKET_KQUEUEEVENTDISPATCHER
#define ROCKET_KQUEUEEVENTDISPATCHER

#include <sys/event.h>

#include "crtp_utils.h"
#include "AbstractEventDispatcher.h"

#define MAX_EVENTS_PER_TICK 10

namespace Rocket {

class KQueueEventDispatcher : public Rocket::AbstractEventDispatcher<KQueueEventDispatcher> {
    IMPLEMENTS(KQueueEventDispatcher, Rocket::AbstractEventDispatcher);

    private:
        KQueueEventDispatcher();

        ~KQueueEventDispatcher();

        static KQueueEventDispatcher* getEventDispatcherImpl();

        void emitImpl(const char* name, void* e);

        void onImpl(const char* name, const Rocket::Events::EventHandler& handler);

        void runImpl();

        void sendEventImpl(const Events::Event* evt);

        static KQueueEventDispatcher* global_instance;

        typedef std::map< std::string, std::deque< Rocket::Events::EventHandler > > EventMap;

        int eventFd;
        int readFd, writeFd;

        struct kevent timer, evtSink;

        EventMap events;

        void setupEventPipe();

        void setupTimer();

};

} // Rocket

#endif // ROCKET_KQUEUEEVENTDISPATCHER
