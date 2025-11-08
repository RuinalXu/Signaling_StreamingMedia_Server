#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <queue>
#include <unistd.h>
#include <semaphore.h>

#include "common.h"
#include "EC_thread.h"

class ThreadTask {
public:
    ThreadTask(){}
    virtual ~ThreadTask(){}
    virtual void run() = 0;
};

/**
 * 定义线程池
 */
class ThreadPool {
private:
    sem_t m_signalSem;
public:
    ThreadPool();
    ~ThreadPool();

    int createThreadPool(int threadCount);

    int waitTask();
    int postTask(ThreadTask* task);

    static void* mainThread(void* argc);

    static std::queue<ThreadTask*> m_taskQueue;

    static pthread_mutex_t m_queueLock;
};

 #endif