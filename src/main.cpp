
#include <private/SimpleEvent.hpp>
#include <private/SimpleEventListener.hpp>
#include <private/SimpleEventSource.hpp>

int main(int argc, char** argv) {
    SimpleEvent e("MyEvent");
    SimpleEventListener l;
    SimpleEventSource s;

    s.addListener(&l);
    s.emit(&e);

    return 0;
}

