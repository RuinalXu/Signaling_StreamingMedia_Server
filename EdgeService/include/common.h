#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>

#include <glog/logging.h>
#include <tinyxml2.h>
#include <json/json.h>

#include "ThreadPool.h"

/**
 * 智能锁,使用系统调用中定义的互斥锁锁,获取到锁后可能出现未释放锁的情况;
 * 因此自定义"智能锁",原因是，私有了该类的构造函数,该对象只能在栈中创建，不能在堆上创建;
 * 创建的对象出栈后,进行析构函数,那么在析构函数函数中释放锁,因此释放该对象时,也释放了锁; 
 * 
 * 用有参构造初始化对象
 */
class AutoMutexLock{
private:
    AutoMutexLock();
    AutoMutexLock(const AutoMutexLock&);
    AutoMutexLock& operator=(const AutoMutexLock&);

    pthread_mutex_t* lock;
public:
    AutoMutexLock(pthread_mutex_t* l): lock(l){ getLock(); };
    ~AutoMutexLock(){ freeLock(); }
    void getLock(){ pthread_mutex_lock(lock); }
    void freeLock(){ pthread_mutex_unlock(lock); }
};

#endif