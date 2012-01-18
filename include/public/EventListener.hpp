#ifndef PUBLIC_EVENTLISTENER_HPP
#define PUBLIC_EVENTLISTENER_HPP

class Event;
class EventSource;

class EventListener {
    public:
        virtual ~EventListener() {}
        virtual void onEvent(const EventSource* src, const Event* e) = 0;
};

#endif //PUBLIC_EVENTLISTENER_HPP
