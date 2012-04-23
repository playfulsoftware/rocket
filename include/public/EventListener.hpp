#ifndef PUBLIC_EVENTLISTENER_HPP
#define PUBLIC_EVENTLISTENER_HPP

class EventSource;

class EventListener {
    public:
        virtual ~EventListener() {}
        virtual void onEvent(const EventSource* src, const char* e) = 0;
};

#endif //PUBLIC_EVENTLISTENER_HPP
