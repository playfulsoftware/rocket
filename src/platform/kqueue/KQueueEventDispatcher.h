#ifndef ROCKET_KQUEUEEVENTDISPATCHER
#define ROCKET_KQUEUEEVENTDISPATCHER

#include "crtp_utils.h"
#include "AbstractEventDispatcher.h"

#define MAX_EVENTS_PER_TICK 10

namespace Rocket {

class KQueueEventDispatcher : public Rocket::AbstractEventDispatcher<KQueueEventDispatcher> {
    IMPLEMENTS(KQueueEventDispatcher, Rocket::AbstractEventDispatcher);

    private:
        KQueueEventDispatcher() {
            // create the event queue
            eventFd = kqueue();

            setupEventPipe();
            setupTimer();
        }

        ~KQueueEventDispatcher() {
            close(eventFd);
            close(readFd);
            close(writeFd);
        }

        static KQueueEventDispatcher* getEventDispatcherImpl();

        void emitImpl(const char* name, void* e) {
            if (events.find(name) != events.end()) {
                std::deque<Rocket::Events::EventHandler>::const_iterator i = events[name].begin();
                for (i; i != events[name].end(); ++i) {
                    (*i)(e);
                }
            }
        }

        void onImpl(const char* name, const Rocket::Events::EventHandler& handler) {
            if (events.find(name) == events.end()) {
                std::deque<Rocket::Events::EventHandler> handlers;
                events[name] = handlers;
            }
            events[name].push_back(handler);
        }

        void runImpl() {
            struct kevent events[MAX_EVENTS_PER_TICK];
            int num_events = 0;
            size_t tickCount;

            tickCount = 0;

            for(;;) {
                num_events = kevent(eventFd, NULL, 0, events, MAX_EVENTS_PER_TICK, NULL);
                if (num_events < 0) {
                    perror("kevent");
                }
                else if (num_events > 0) {
                    // process events
                    for (int i = 0; i < num_events; i++) {
                        switch(events[i].filter) {
                            case EVFILT_TIMER:
                                if (events[i].ident == 1) { // core timer
                                    emit("onTick", (void*)tickCount);
                                }
                                break;
                            case EVFILT_READ:
                                if (events[i].ident == readFd) { // there are pending events
                                    size_t pending = events[i].data;
                                    Events::Event evt;
                                    while (pending >= sizeof(Events::Event)) {
                                        ssize_t bytes = read(readFd, &evt, sizeof(Events::Event));
                                        if (bytes < sizeof(Events::Event)) {
                                            printf("Error reading event.. partial read.");
                                        } else {
                                            emit("onEvent", &evt);
                                        }
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                tickCount++;
            }
        }

        void sendEventImpl(const Events::Event* evt) {
            write(writeFd, evt, sizeof(evt));
        }

        static KQueueEventDispatcher* global_instance;

        typedef std::map< std::string, std::deque< Rocket::Events::EventHandler > > EventMap;

        int eventFd;
        int readFd, writeFd;

        struct kevent timer, evtSink;

        EventMap events;

        void setupEventPipe() {
            // create the event source / sink descriptors
            int pipeFds[2] = {0, 0};
            pipe(pipeFds);
            readFd = pipeFds[0];
            writeFd = pipeFds[1];

            EV_SET(&evtSink, readFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
            struct timespec ts = {0, 0};
            kevent(eventFd, &evtSink, 1, NULL, 0, &ts);
        }

        void setupTimer() {
            // initialize the Timer struct
            EV_SET(&timer, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 16, 0);
            // register the timer with the event queue
            struct timespec ts = {0, 0};
            kevent(eventFd, &timer, 1, NULL, 0, &ts);
        }

};

KQueueEventDispatcher* KQueueEventDispatcher::global_instance = NULL;

KQueueEventDispatcher* KQueueEventDispatcher::getEventDispatcherImpl() {
    if (global_instance == NULL) {
        printf("Creating new KQueue Event Dispatcher\n");
        global_instance = new KQueueEventDispatcher;
    }
    return global_instance;
}


} // Rocket

#endif // ROCKET_KQUEUEEVENTDISPATCHER
