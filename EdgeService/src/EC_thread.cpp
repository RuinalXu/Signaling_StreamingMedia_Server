#include "EC_thread.h"

using namespace embedded_controller;

int ECThread::createThread(ECThreadFunc startRoutine, void* args, pthread_t& id) {
    // 定义线程的属性
    int ret = 0;
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    // 对线程属性进行设定
    do {
        ret = pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
        if (ret != 0) {
            break;
        }
        ret = pthread_create(&id, &threadAttr, startRoutine, args);
        if (ret != 0) {
            break;
        }
    } while(0);
    // 销毁线程
    pthread_attr_destroy(&threadAttr);
    if (ret != 0) {
        ret = -1;
    }
    return ret;
}

/**
 * 分离当前线程
 */
int ECThread::detachSelf() {
    // pthread_self()获取当先线程的线程id
    int ret = pthread_detach(pthread_self());
    if (ret != 0) {
        ret = -1;
    }
    return 0;
}
/**
 * 退出当前线程
 */
void ECThread::exitSelf(void* rval) {
    pthread_exit(rval);
}

/**
 * 阻塞等待指定的线程退出
 */
int ECThread::waitThread(const pthread_t& id, void** rval) {
    int ret = pthread_join(id, rval);
    if (ret!= 0) {
        return -1;
    }
    return 0;
}

/**
 * 向指定的线程发送信号
 */
int ECThread::terminateThread(const pthread_t& id) {
    int ret = pthread_cancel(id);
    if (ret != 0) {
        return -1;
    }
    return 0;
}