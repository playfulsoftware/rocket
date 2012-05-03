#ifndef PRIVATE_SIMPLEEVENTSOURCE_HPP
#define PRIVATE_SIMPLEEVENTSOURCE_HPP

#include <iostream>
#include <vector>

#include "EventListener.hpp"
#include "EventSource.hpp"

using namespace std;


class SimpleEventSource : public EventSource {
    private:
        vector<EventListener*> listeners;

    public:
        virtual bool addListener(EventListener* listener) {
            listeners.push_back(listener);
            std::cout << "Added Listener" << std::endl;
            return true;
        }

        virtual bool removeListener(EventListener* listener) {
            vector<EventListener*>::iterator iter;

            for (iter = listeners.begin(); iter < listeners.end(); iter++) {
                if (listener == *iter) {
                    listeners.erase(iter);
                    return true;
                }
            }

            return false;
        }

        virtual void emit(const char* e) {
            vector<EventListener*>::iterator iter;

            for (iter = listeners.begin(); iter < listeners.end(); iter++) {
                (*iter)->onEvent(this, e);
            }
        }
};

#endif //PRIVATE_SIMPLEEVENTSOURCE_HPP
