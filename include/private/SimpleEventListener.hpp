#ifndef PRIVATE_SIMPLEEVENTLISTENER_HPP
#define PRIVATE_SIMPLEEVENTLISTENER_HPP

#include <iostream>

#include <public/Event.hpp>
#include <public/EventListener.hpp>
#include <public/EventSource.hpp>

class SimpleEventListener : public EventListener {
    public:
        virtual void onEvent(const EventSource* src, const Event* e) {
            std::cout << "saw an event: " << e->getName() << std::endl;
        }
};
#endif //PRIVATE_SIMPLEEVENTLISTENER_HPP
