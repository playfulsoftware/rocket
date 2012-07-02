#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

class EventDispatcher {
    public:
        EventDispatcher() {
            // create the event queue
            eventFd = kqueue();

        }

        ~EventDispatcher() {
            close(eventFd);
        }

        void run() {
            struct kevent timer = { 0 }, event = { 0 };
            int num_events, tickCount;

            tickCount = 0;

            // initialize the Timer struct
            EV_SET(&timer, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 16, 0);

            for(;;) {
                num_events = kevent(eventFd, &timer, 1, &event, 1, NULL);
                if (num_events < 0) {
                    perror("kevent");
                }
                else if (num_events > 0) {
                    printf("timer tick %d\n", tickCount++);
                }
            }
        }

    private:
        int eventFd;
};

int main(int argc, char** argv) {

    EventDispatcher ed;

    ed.run();

    return 0;
}

