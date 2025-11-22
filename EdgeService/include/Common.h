#ifndef COMMON_H
#define COMMON_H

#include <glog/logging.h>
#include <tinyxml2.h>
#include <json/json.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <semaphore.h>
#include <memory>
#include <sstream>

using namespace std;

/**
 * 智能锁,使用系统调用中定义的互斥锁锁,获取到锁后可能出现未释放锁的情况;
 * 因此自定义"智能锁",原因是，私有了该类的构造函数,该对象只能在栈中创建，不能在堆上创建;
 * 创建的对象出栈后,进行析构函数,那么在析构函数函数中释放锁,因此释放该对象时,也释放了锁; 
 * 
 * 用有参构造初始化对象
 */
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
