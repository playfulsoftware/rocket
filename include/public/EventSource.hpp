#ifndef PUBLIC_EVENTSOURCE_HPP
#define PUBLIC_EVENTSOURCE_HPP

class EventListener;

class EventSource {
    public:
        virtual ~EventSource() {}
        virtual bool addListener(EventListener* listener) = 0;
        virtual bool removeListener(EventListener* listener) = 0;
        virtual void emit(const char* e) = 0;
};


#endif //PUBLIC_EVENTSOURCE_HPP
