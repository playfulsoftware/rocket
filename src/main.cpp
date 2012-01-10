#include <cstdio>

#include <iostream>

#include <vector>

using namespace std;

class Event {
    public:
        virtual ~Event() {}
        virtual string getName() const = 0;
};

class EventSource;

class EventListener {
    public:
        virtual ~EventListener() {}
        virtual void onEvent(EventSource& src, const Event& e) = 0;
};

class EventSource {
    public:
        virtual ~EventSource() {}
        virtual bool addListener(EventListener* listener) = 0;
        virtual bool removeListener(EventListener* listener) = 0;
        virtual void emit(const Event& e) = 0;
};

class SimpleEvent : public Event {
    private:
        string _name;
    public:
        SimpleEvent(string name) : _name(name) {}

        virtual string getName() const { return _name; }
};

class SimpleEventListener : public EventListener {
    public:
        virtual void onEvent(EventSource& src, const Event& e) {
            cout << "saw an event: " << e.getName() << endl;
        }
};

class SimpleEventSource : public EventSource {
    private:
        vector<EventListener*> listeners;

    public:
        virtual bool addListener(EventListener* listener) {
            listeners.push_back(listener);
            cout << "Added Listener" << endl;
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

        virtual void emit(const Event& e) {
            vector<EventListener*>::iterator iter;

            for (iter = listeners.begin(); iter < listeners.end(); iter++) {
                (*iter)->onEvent(*this, e);
            }
        }
};

int main(int argc, char** argv) {
    SimpleEvent e("MyEvent");
    SimpleEventListener l;
    SimpleEventSource s;

    s.addListener(&l);
    s.emit(e);

    return 0;
}

