#ifndef COMMON_H
#define COMMON_H

#include <glog/logging.h>
#include "tinyxml2.h"
#include <json/json.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <semaphore.h>
#include <memory>
#include <sstream>

using namespace std;

class AutoMutexLock {
public:
    AutoMutexLock(pthread_mutex_t* l): lock(l) {
        LOG(INFO) << "get lock";
        getLock();
    }

    ~AutoMutexLock() {
        LOG(INFO) << "free lock";
    }
private:
    pthread_mutex_t* lock;
    AutoMutexLock();
    AutoMutexLock(const AutoMutexLock&);
    
    /**
     * 获取锁
     */
    void getLock(){ pthread_mutex_lock(lock); }

    /**
     * 释放锁
     */
    void freeLock() { pthread_mutex_unlock(lock); }
};

#endif