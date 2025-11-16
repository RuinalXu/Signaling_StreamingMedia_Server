#include "ThreadPool.h"

// pthread_mutex_t ThreadPool::m_queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::m_queueLock;
std::queue<ThreadTask*> ThreadPool::m_taskQueue;

ThreadPool::ThreadPool() {
    pthread_mutex_init(&m_queueLock, NULL);
    sem_init(&m_signalSem, 0, 0);
}

ThreadPool::~ThreadPool() {
    pthread_mutex_destroy(&m_queueLock);
    sem_destroy(&m_signalSem);
}

/**
 * 创建线程池
 * 
 * @param threadCount 线程池中线程数量
 */
int ThreadPool::createThreadPool(int threadCount) {
    if (threadCount <= 0) {
        LOG(ERROR) << " thread count error";
        return -1;
    }
    for (int i = 0; i < threadCount; i++) {
        pthread_t pid;
        if (embedded_controller::ECThread::createThread(ThreadPool::mainThread, (void*)this, pid) < 0) {
            LOG(ERROR) << "create thread error";
        }
        LOG(INFO) << "thread:" << pid << " was created";
    }
    return 0;
}

/**
 * 线程入口函数
 */
void* ThreadPool::mainThread(void* argc) {
    ThreadPool* pthis = (ThreadPool*) argc;
    do {
        int ret = pthis -> waitTask();
        if (ret == 0) {
            ThreadTask* task = NULL;
            // 加互斥锁
            pthread_mutex_lock(&m_queueLock);
            if (m_taskQueue.size() > 0) {
                task = m_taskQueue.front();
                m_taskQueue.pop();
            }
            // 解锁
            pthread_mutex_unlock(&m_queueLock);
            if (task) {
                task -> run();
                delete task;
            }
        }
        
    } while (true);
}

int ThreadPool::waitTask(){
    int ret = 0;
    ret = sem_wait(&m_signalSem);
    if (ret != 0) {
        LOG(ERROR) << "the api execute error";
    }
    return ret;
}

int ThreadPool::postTask(ThreadTask* task) {
    if (task) {
        pthread_mutex_lock(&m_queueLock);
        m_taskQueue.push(task);
        pthread_mutex_unlock(&m_queueLock);
        sem_post(&m_signalSem);
    }
}
