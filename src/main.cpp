#include <pthread.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <deque>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "fastdelegate.h"

#include "events/EventEmitter.h"

#define MAX_EVENTS_PER_TICK 10

#define EVENT_SIZE 256

typedef enum {
    MESSAGE_EVENT
} EventType;

typedef struct _Event {
    EventType type;
    void* data[EVENT_SIZE - sizeof(EventType)];
} Event;

class KQueueEventDispatcher : public Rocket::Events::EventEmitter {
    public:
        KQueueEventDispatcher() {
            // create the event queue
            eventFd = kqueue();

            setupEventPipe();
            setupTimer();
        }

        ~KQueueEventDispatcher() {
            close(eventFd);
            close(readFd);
            close(writeFd);
        }

        void emit(const char* name, void* e) {
            if (events.find(name) != events.end()) {
                std::deque<Rocket::Events::EventHandler>::const_iterator i = events[name].begin();
                for (i; i != events[name].end(); ++i) {
                    (*i)(e);
                }
            }
        }

        virtual void on(const char* name, const Rocket::Events::EventHandler& handler) {
            if (events.find(name) == events.end()) {
                std::deque<Rocket::Events::EventHandler> handlers;
                events[name] = handlers;
            }
            events[name].push_back(handler);
        }

        void run() {
            struct kevent events[MAX_EVENTS_PER_TICK];
            int num_events = 0;
            size_t tickCount;

            tickCount = 0;

            for(;;) {
                num_events = kevent(eventFd, NULL, 0, events, MAX_EVENTS_PER_TICK, NULL);
                if (num_events < 0) {
                    perror("kevent");
                }
                else if (num_events > 0) {
                    // process events
                    for (int i = 0; i < num_events; i++) {
                        switch(events[i].filter) {
                            case EVFILT_TIMER:
                                if (events[i].ident == 1) { // core timer
                                    emit("onTick", (void*)tickCount);
                                }
                                break;
                            case EVFILT_READ:
                                if (events[i].ident == readFd) { // there are pending events
                                    size_t pending = events[i].data;
                                    Event evt;
                                    while (pending >= EVENT_SIZE) {
                                        ssize_t bytes = read(readFd, &evt, sizeof(Event));
                                        if (bytes < sizeof(Event)) {
                                            printf("Error reading event.. partial read.");
                                        } else {
                                            emit("onEvent", &evt);
                                        }
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                tickCount++;
            }
        }

        void sendEvent(const Event* evt) {
            write(writeFd, evt, sizeof(evt));
        }


    private:
        typedef std::map< std::string, std::deque< Rocket::Events::EventHandler > > EventMap;

        int eventFd;
        int readFd, writeFd;

        struct kevent timer, evtSink;

        EventMap events;

        void setupEventPipe() {
            // create the event source / sink descriptors
            int pipeFds[2] = {0, 0};
            pipe(pipeFds);
            readFd = pipeFds[0];
            writeFd = pipeFds[1];

            EV_SET(&evtSink, readFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
            struct timespec ts = {0, 0};
            kevent(eventFd, &evtSink, 1, NULL, 0, &ts);
        }

        void setupTimer() {
            // initialize the Timer struct
            EV_SET(&timer, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 16, 0);
            // register the timer with the event queue
            struct timespec ts = {0, 0};
            kevent(eventFd, &timer, 1, NULL, 0, &ts);
        }

};

typedef fastdelegate::FastDelegate<void (void *)> WorkFunc;

class PthreadThreadPool {

    public:
        struct WorkItem {
            WorkFunc func;
            void* data;
        };

        struct WorkerThread {
            pthread_t threadId;
        };

        PthreadThreadPool(int maxThreads) {

            // initialize the lock for the task queue
            pthread_mutex_init(&taskLock, NULL);
            pthread_mutex_init(&threadLock, NULL);
            

            this->maxThreads = maxThreads;

            // initialize the starting thread pool to 20% of the max thread 
            // count or 1 thread, whichever is larger.
            int startThreads = this->maxThreads / 5;
            startThreads = (startThreads < 1) ? 1 : startThreads;

            for (int i = 0; i < startThreads; i++) {
                startWorkerThread();
            }
        }

        ~PthreadThreadPool() {
            isActive = false;
            std::vector<WorkerThread*>::const_iterator i = threadList.begin();
            for(i; i != threadList.end(); ++i) {
                // wait for the pooled threads to finish..
                pthread_join((*i)->threadId, NULL);
            }
        }

        void queueTask(WorkFunc func, void* data) {
            struct WorkItem item;
            item.func = func;
            item.data = data;
            queueTask(item);
        }

        void queueTask(WorkItem work) {
            pthread_mutex_lock(&taskLock);
            workList.push(work);
            pthread_mutex_unlock(&taskLock);

            // check to see if the thread pool needs to grow
            if (threadList.size() < maxThreads && workList.size() > (threadList.size() * 2))
            {
                pthread_mutex_lock(&threadLock);
                startWorkerThread();
                pthread_mutex_unlock(&threadLock);
            }
        }

    private:
        void startWorkerThread() {
            WorkerThread* thread = NULL;
            thread = new WorkerThread;
            pthread_create(&(thread->threadId), NULL, workerThreadLoop, this);
            threadList.push_back(thread);
        }

        static void* workerThreadLoop(void* rawPool) {
            PthreadThreadPool* pool = (PthreadThreadPool*)rawPool;
            while (pool->isActive) {
                if (pool->workList.empty()) {
                    usleep(10);
                } else {
                    pthread_mutex_lock(&(pool->taskLock));
                    WorkItem work = pool->workList.front();
                    pool->workList.pop();
                    pthread_mutex_unlock(&(pool->taskLock));
                    work.func(work.data);
                }
            }
            return NULL;
        }

        bool isActive;
        int maxThreads;
        pthread_mutex_t taskLock;
        pthread_mutex_t threadLock;
        std::queue<WorkItem> workList;
        std::vector<WorkerThread*> threadList;

};

void printTicks(void* data) {
    printf("timer tick %lu\n", (size_t)data);
}


void waitAndPing(void* data) {
    KQueueEventDispatcher* ed = (KQueueEventDispatcher*)data;

    sleep(5);

    Event evt_data; 

    ed->sendEvent(&evt_data);
}

int main(int argc, char** argv) {

    KQueueEventDispatcher ed;

    ed.on("onTick", printTicks);

    ed.run();

    return 0;
}

