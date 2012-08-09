#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <deque>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "platform/kqueue/KQueueEventDispatcher.h"

namespace Rocket {

KQueueEventDispatcher::KQueueEventDispatcher() {
    // create the event queue
    eventFd = kqueue();

    // set up an event pipe
    setupEventPipe();

    // set up an event timer
    setupTimer();
}

KQueueEventDispatcher::~KQueueEventDispatcher() {
    close(eventFd);
    close(readFd);
    close(writeFd);
}

KQueueEventDispatcher* KQueueEventDispatcher::global_instance = NULL;

KQueueEventDispatcher* KQueueEventDispatcher::getEventDispatcherImpl() {
    if (global_instance == NULL) {
        printf("Creating new KQueue Event Dispatcher\n");
        global_instance = new KQueueEventDispatcher;
    }
    return global_instance;
}

void KQueueEventDispatcher::emitImpl(const char* name, void* e) {
    if (events.find(name) != events.end()) {
        std::deque<Rocket::Events::EventHandler>::const_iterator i = events[name].begin();
        for (i; i != events[name].end(); ++i) {
            (*i)(e);
        }
    }
}

void KQueueEventDispatcher::onImpl(const char* name, const Rocket::Events::EventHandler& handler) {
    if (events.find(name) == events.end()) {
        std::deque<Rocket::Events::EventHandler> handlers;
        events[name] = handlers;
    }
    events[name].push_back(handler);
}

void KQueueEventDispatcher::runImpl() {
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

void KQueueEventDispatcher::sendEventImpl(const Events::Event* evt) {
    write(writeFd, evt, sizeof(evt));
}

void KQueueEventDispatcher::setupEventPipe() {
    // create the event source / sink descriptors
    int pipeFds[2] = {0, 0};
    pipe(pipeFds);
    readFd = pipeFds[0];
    writeFd = pipeFds[1];

    EV_SET(&evtSink, readFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    struct timespec ts = {0, 0};
    kevent(eventFd, &evtSink, 1, NULL, 0, &ts);
}

void KQueueEventDispatcher::setupTimer() {
    // initialize the Timer struct
    EV_SET(&timer, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 16, 0);
    // register the timer with the event queue
    struct timespec ts = {0, 0};
    kevent(eventFd, &timer, 1, NULL, 0, &ts);
}

} // Rocket

