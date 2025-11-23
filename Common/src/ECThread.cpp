#include "ECThread.h"

using namespace EC;

/**
 *  创建detached线程,该线程不能被join,资源自动回收
 */
int ECThread::createThread(ECThreadFunc startRoutine, void* args, pthread_t& id){
    int ret = 0;
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    // do-while(0),模拟"goto cleanup"逻辑,避免重复destroy
    do {
        ret = pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
        if (ret != 0){
            break;
        }
        ret = pthread_create(&id, &threadAttr, startRoutine, args);
        if (ret != 0){
            break;
        }
    } while(0);
    pthread_attr_destroy(&threadAttr);
    if (ret != 0) {
        ret = -1;
    }
    return ret;
}

/**
 *  将当前线程设置为detached
 */
int ECThread::detachSelf(){
    int ret = pthread_detach(pthread_self());
    if (ret != 0){
        return -1;
    }
    return 0;
}

/**
 *  退出当前线程,并返回回调参数
 */
void ECThread::exitSelf(void* rval){
    pthread_exit(rval);
}

/**
 *  阻塞当前线程
 */
int ECThread::waitThread(const pthread_t& id, void** rval){
    int ret = pthread_join(id, rval);
    if (ret != 0){
        return -1;
    }
    return 0;
}

/**
 *  请求取消线程
 */
int ECThread::terminateThread(const pthread_t& id){
    int ret = pthread_cancel(id);
    if (ret != 0){
        return -1;
    }
    return 0;
}
