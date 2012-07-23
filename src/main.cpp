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

#include "events/EventDispatcher.h"
#include "events/EventEmitter.h"

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
    Rocket::EventDispatcher* ed = (Rocket::EventDispatcher*)data;

    sleep(5);

    Rocket::Events::Event evt_data; 

    ed->sendEvent(&evt_data);
}

int main(int argc, char** argv) {
    printf("Getting Event Dispatcher\n");
    Rocket::EventDispatcher* ed = Rocket::EventDispatcher::getEventDispatcher();

    if (ed == NULL) {
        printf("Got a NULL Event Dispatcher\n");
    }

    printf("Registering Event\n");
    ed->on("onTick", printTicks);

    printf("Running Event Dispatcher\n");
    ed->run();

    return 0;
}

