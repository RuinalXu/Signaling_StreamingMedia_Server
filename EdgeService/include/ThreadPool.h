#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "Common.h"
#include "ECThread.h"
#include <queue>
#include <unistd.h>
#include <semaphore.h>

using namespace EC;

class ThreadTask {
public:
    ThreadTask(){}
    virtual ~ThreadTask(){}
    virtual void run() = 0;
};

class ThreadPool {
private:
    sem_t m_signalSem;
public:
    static queue<ThreadTask*> m_taskQueue;
    static pthread_mutex_t m_queueLock;

    ThreadPool();
    ~ThreadPool();

    static void* mainThread(void* argc);
    
    int createThreadPool(int threadCount);
    int waitTask();
    int postTask(ThreadTask* task);

};

#endif
