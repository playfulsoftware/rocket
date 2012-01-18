#ifndef PRIVATE_SIMPLEEVENT_HPP
#define PRIVATE_SIMPLEEVENT_HPP
#include <string>

#include <public/Event.hpp>

using namespace std;

class SimpleEvent : public Event {
    private:
        string _name;
    public:
        SimpleEvent(string name) : _name(name) {}

        virtual string getName() const { return _name; }

};
#endif //PRIVATE_SIMPLEVENT_HPP
