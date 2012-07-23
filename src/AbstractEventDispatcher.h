#ifndef ROCKET_ABSTRACTEVENTDISPATCHER_H
#define ROCKET_ABSTRACTEVENTDISPATCHER_H

#include "events/Event.h"
#include "events/EventEmitter.h"
#include "events/EventHandler.h"

namespace Rocket {

template <typename Impl>
class AbstractEventDispatcher : public Rocket::Events::EventEmitter {
    public:
        static Impl* getEventDispatcher();

        void emit(const char* name, void* e) {
            static_cast<Impl*>(this)->emitImpl(name, e);
        }

        void on(const char* name, const Rocket::Events::EventHandler& handler) {
            static_cast<Impl*>(this)->onImpl(name, handler);
        }

        void run() {
            static_cast<Impl*>(this)->runImpl();
        }

        void sendEvent(const Events::Event* e) {
            static_cast<Impl*>(this)->sendEventImpl(e);
        }
};

template <typename Impl>
Impl* AbstractEventDispatcher<Impl>::getEventDispatcher() {
    return Impl::getEventDispatcherImpl();
} 

} // Rocket

#endif // ROCKET_ABSTRACTEVENTDISPATCHER_H
