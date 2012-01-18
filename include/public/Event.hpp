#ifndef PUBLIC_EVENT_HPP
#define PUBLIC_EVENT_HPP

#include <string>

using namespace std;

class Event {
    public:
        virtual ~Event() {}
        virtual string getName() const = 0;
};

#endif //PUBLIC_EVENT_HPP
